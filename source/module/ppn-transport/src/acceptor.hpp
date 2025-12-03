// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport
{
    class Acceptor
        : public api::Acceptor<>::Opposite
        , public host::module::ServiceBase<Acceptor>
    {
    public:
        Acceptor();
        ~Acceptor();

    private:
        struct Downstream
        {
            Downstream(api::acceptor::Downstream<>&& instance, Acceptor* upstream);
            ~Downstream();

            void start();
            void stop();

            api::acceptor::Downstream<> _instance;
            sbs::Owner                  _sbsOwner;
            bool                        _needStart = false;
            bool                        _started = false;
            size_t                      _failed = 0;

            dci::poll::Timer            _restartTicker;
        };
        using DownstreamPtr = std::unique_ptr<Downstream>;

    private:
        using Downstreams = List<DownstreamPtr>;
        Downstreams _downstreams;

        bool _started {false};
    };
}
