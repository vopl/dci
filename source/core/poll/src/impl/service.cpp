/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
