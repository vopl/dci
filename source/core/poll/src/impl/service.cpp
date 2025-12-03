// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "service.hpp"
#include <dci/poll/descriptor.hpp>
#include <dci/poll/error.hpp>
#include <dci/cmt.hpp>

namespace dci::poll::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::Service()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::~Service()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Service::initialize()
    {
        return _polling.initialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Service::run(bool emitStartedStopped)
    {
        if(!_stop)
        {
            return error::already_started;
        }

        if(!_polling.initialized())
        {
            return error::not_initialized;
        }

        _stop = false;

        if(emitStartedStopped)
        {
            _started.in();
        }

        while(!_stop)
        {
            _clocking.fireTicks();

            bool someWorkDone = false;
            if(_doSomeWork.connected())
                someWorkDone = _doSomeWork.in();

            if(someWorkDone && _clocking.fireTicks())
            {
                continue;
            }

            if(_awaking.woken())
            {
                continue;
            }

            if(someWorkDone)
            {
                continue;
            }

            if(!_polling.hasPayload() && !_clocking.hasPayload() && !_awaking.hasPayload())
            {
                _stop = true;
                break;
            }

            clocking::Duration timeout = _clocking.distance2NextPoint();
            if(!timeout.count())
            {
                continue;
            }

            auto ec = _polling.execute(timeout);
            if(ec)
            {
                if(ec == std::errc::interrupted)
                {
                    _awaking.woken();
                    continue;
                }

                if(_stop && ec == error::not_initialized)
                {
                    break;
                }

                if(emitStartedStopped)
                {
                    _stopped.in();
                }

                return ec;
            }
        }

        if(_doSomeWork.connected())
            _doSomeWork.in();

        if(emitStartedStopped)
        {
            _stopped.in();
        }

        return std::error_code{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Service::started()
    {
        return _started.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<bool> Service::doSomeWork()
    {
        return _doSomeWork.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Service::stop()
    {
        if(_stop)
        {
            return error::already_stopped;
        }

        _stop = true;
        return _polling.wakeup();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Service::stopped()
    {
        return _stopped.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::error_code Service::deinitialize()
    {
        if(!_stop)
        {
            return error::not_stopped;
        }

        return _polling.deinitialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Clocking& Service::clocking()
    {
        return _clocking;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Polling& Service::polling()
    {
        return _polling;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaking& Service::awaking()
    {
        return _awaking;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        alignas(Service) std::byte servicePlace[sizeof(Service)];
    }

    Service& service{*(new (&servicePlace) Service)};
}
