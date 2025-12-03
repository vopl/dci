// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "acceptor.hpp"

namespace dci::module::ppn::transport
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::Acceptor()
        : api::Acceptor<>::Opposite(idl::interface::Initializer())
    {
        //out accepted(Channel);
        //in start();
        methods()->start() += serviceSol() * [this]
        {
            if(!_started)
            {
                _started = true;

                for(std::size_t i(0); i<_downstreams.size(); ++i)
                {
                    _downstreams[i]->start();
                }
            }
        };

        //in stop()
        methods()->stop() += serviceSol() * [this]
        {
            if(_started)
            {
                _started = false;

                for(std::size_t i(0); i<_downstreams.size(); ++i)
                {
                    _downstreams[i]->stop();
                }
            }
        };

        //out started(transport::Address);
        //out stopped(transport::Address);
        //out failed(transport::Address, exception);

        //in add(Acceptor);
        methods()->add() += serviceSol() * [this](api::acceptor::Downstream<>&& instance)
        {
            const DownstreamPtr& dsp = _downstreams.emplace_back(std::make_unique<Downstream>(std::move(instance), this));

            if(_started)
            {
                dsp->start();
            }
        };

        //in del(Acceptor);
        methods()->del() += serviceSol() * [this](api::acceptor::Downstream<>&& instance)
        {
            for(std::size_t i(0); i<_downstreams.size(); ++i)
            {
                if(_downstreams[i]->_instance == instance)
                {
                    DownstreamPtr dsp = std::move(_downstreams[i]);
                    _downstreams.erase(_downstreams.begin() + static_cast<std::ptrdiff_t>(i));
                    dsp->stop();
                    dsp.reset();
                    break;
                }
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::~Acceptor()
    {
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::Downstream::Downstream(api::acceptor::Downstream<>&& instance, Acceptor* upstream)
        : _instance(std::move(instance))
    {
        _instance->accepted() += _sbsOwner * [&, upstream](api::Channel<>&& ch)
        {
            _failed = 0;
            _restartTicker.stop();
            (*upstream)->accepted(std::move(ch));
        };

        _instance->started() += _sbsOwner * [&, upstream](api::Address&& addr1, api::Address&& addr2)
        {
            _started = true;
            _failed = 0;
            _restartTicker.stop();
            (*upstream)->started(std::move(addr1), std::move(addr2));
        };

        _instance->stopped() += _sbsOwner * [&, upstream](api::Address&& addr1, api::Address&& addr2)
        {
            _started = false;
            (*upstream)->stopped(std::move(addr1), std::move(addr2));

            if(_needStart)
            {
                _failed++;
                uint64 ms = std::min(60000ull, 200ull + (1ull << _failed));
                _restartTicker.interval(std::chrono::milliseconds{ms});
                _restartTicker.start();
            }
            else
            {
                _failed = 0;
                _restartTicker.stop();
            }
        };

        _instance->failed() += _sbsOwner * [&, upstream](api::Address&& addr1, api::Address&& addr2, ExceptionPtr&& e)
        {
            (*upstream)->failed(std::move(addr1), std::move(addr2), std::move(e));

            if(_needStart)
            {
                _failed++;
                uint64 ms = std::min(60000ull, 200ull + (1ull << _failed));
                _restartTicker.interval(std::chrono::milliseconds{ms});
                _restartTicker.start();
            }
        };

        _restartTicker.tick() += [this]
        {
            if(_needStart && _failed && !_started)
            {
                _instance->stop();
                _instance->start();
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Acceptor::Downstream::~Downstream()
    {
        _restartTicker.stop();
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Acceptor::Downstream::start()
    {
        _needStart = true;
        _failed = 0;
        _restartTicker.stop();
        _instance->start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Acceptor::Downstream::stop()
    {
        _needStart = false;
        _failed = 0;
        _restartTicker.stop();
        _instance->stop();
    }
}
