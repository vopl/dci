// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../service.hpp"

namespace dci::module::ppn::transport::natt::mapper::netBased
{
    template <class MostLookout, class MostService, class MostPerformer>
    class Service
        : public mapper::Service
    {
    public:
        using Clock = std::chrono::steady_clock;
        using Timepoint = Clock::time_point;

        struct State
        {
            Timepoint       _lastTried {};
            Timepoint       _lastFound {};
            Timepoint       _lastFailed {};
            std::size_t     _ban {};

            void fixTried(Timepoint tp);
            void fixFound(Timepoint tp);
            void fixFailed(Timepoint tp);
            void fixBan();

            Timepoint nextReveal() const;
            bool ok() const;

            void reset();
        };

    public:
        static constexpr std::chrono::milliseconds _timeoutReveal               {1000 * 60 * 10};
        static constexpr std::chrono::milliseconds _timeoutRevealAfterFail      {1000 * 60};
        static constexpr std::chrono::milliseconds _timeoutRevealAmortization   {250};
        static constexpr std::chrono::milliseconds _timeoutRevealUrgent         {250};
        static constexpr std::chrono::milliseconds _timeoutRevealAnormal        {1000 * 60};

        static constexpr std::chrono::milliseconds _timeoutInteractMinimal      {57};
        static constexpr std::chrono::milliseconds _timeoutInteractMaximal      {1000 * 2};

    public:
        Service(MostLookout* l, const net::IpAddress& clientAddr, const net::Endpoint& srvEp, std::chrono::milliseconds startAfter = _timeoutRevealUrgent);
        ~Service() override;

        PerformerBlank candidate(const apit::Address& internal) override;

        cmt::Waitable& serviceChangedWaitable();

    protected:
        void reveal();

        bool send(auto&& requestMaker);

        enum class InteractResult
        {
            null,
            fail,
            partial,
            success
        };

        InteractResult interact(auto&& requestMaker, auto&& responseHandler);

    protected:
        MostLookout *               _l {};
        net::IpAddress              _clientAddr;
        net::Endpoint               _srvEp;

        cmt::task::Owner            _tol;
        poll::Timer                 _revealTicker;
        std::size_t                 _revealDepth {};

        sbs::Owner                  _clientSol;
        net::datagram::Channel<>    _client;
        net::Endpoint               _clientEp;
        cmt::Mutex                  _interactMtx;

        cmt::Pulser                 _serviceChanged;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    bool Service<MostLookout, MostService, MostPerformer>::send(auto&& requestMaker)
    {
        if(!_client) return false;

        Bytes request = requestMaker();
        if(request.empty()) return false;

        _client->send(std::move(request), _srvEp);

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    typename Service<MostLookout, MostService, MostPerformer>::InteractResult Service<MostLookout, MostService, MostPerformer>::interact(auto&& requestMaker, auto&& responseHandler)
    {
        std::scoped_lock l(_interactMtx);

        std::chrono::milliseconds timeout{_timeoutInteractMinimal};
        std::size_t renews = 4;
        for(std::size_t i{0}; i<10; ++i)
        {
            if(!send(requestMaker)) return InteractResult::null;

            if(!_client) return InteractResult::null;

            utils::S2f resp{_client->received()};

            poll::WaitableTimer t{timeout};
            t.start();

            switch(cmt::waitAny(resp, t, _serviceChanged))
            {
            case 0://resp
                {
                    auto&&[data, ep] = resp.detachValue();
                    if(ep != _srvEp)
                    {
                        continue;
                    }

                    switch(responseHandler(std::move(data)))
                    {
                    case InteractResult::null:
                        break;

                    case InteractResult::fail:
                        return InteractResult::fail;

                    case InteractResult::partial:
                        if(renews)
                        {
                            --renews;
                            timeout = _timeoutInteractMinimal;
                            i = 0;
                        }
                        break;

                    case InteractResult::success:
                        return InteractResult::success;
                    }
                }
                break;

            case 1://timeout
                break;

            case 2://serviceChanged
                if(renews)
                {
                    --renews;
                    timeout = _timeoutInteractMinimal;
                    i = 0;
                }
                break;
            }

            timeout = std::min(timeout * 2, _timeoutInteractMaximal);
        }

        return InteractResult::null;
    }
}
