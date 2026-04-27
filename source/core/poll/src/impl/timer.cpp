/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "timer.hpp"
#include "service.hpp"
#include <dci/cmt/functions.hpp>
#include <dci/utils/atScopeExit.hpp>

namespace dci::poll::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(Duration interval,
                     bool repeatable,
                     cmt::Raisable* raisable)
        : _interval{interval.count()}
        , _repeatable{repeatable}
        , _tickRaisable{raisable}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::~Timer()
    {
        stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Timer::tick()
    {
        return _tickWire.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::setRaisable(cmt::Raisable* raisable)
    {
        _tickRaisable = raisable;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::resetRaisable()
    {
        _tickRaisable = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Duration Timer::remaining() const
    {
        if(!started())
        {
            return Duration{-1};
        }

        PointRep now = service.clocking().exactNow();

        if(_point <= now)
        {
            return Duration{};
        }

        return Duration{_point - now};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Duration Timer::interval() const
    {
        return Duration{_interval};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::interval(Duration v)
    {
        DurationRep interval = v.count();

        if(started())
        {
            if(_interval >= 0)
            {
                if(interval >= 0)
                {
                    _point = service.clocking().exactNow() + interval;
                    service.clocking().update(this);
                }
                else
                {
                    service.clocking().stop(this);
                }
            }
            else
            {
                if(interval >= 0)
                {
                    _point = service.clocking().exactNow() + interval;
                    service.clocking().start(this);
                }
                else
                {
                    //nothing
                }
            }
        }

        _interval = interval;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timer::repeatable() const
    {
        return _repeatable;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::repeatable(bool v)
    {
        _repeatable = v;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::start()
    {
        dbgAssert(0 <= _interval);
        if(!_started && 0 <= _interval)
        {
            _point = service.clocking().exactNow() + _interval;
            service.clocking().start(this);
            _started = true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::restart()
    {
        stop();
        start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timer::started() const
    {
        return _started;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::stop()
    {
        if(_started)
        {
            if(_interval >= 0)
            {
                service.clocking().stop(this);
            }
            _started = false;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::PointRep Timer::nextPoint() const
    {
        return _point;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::tick(PointRep now)
    {
        dbgAssert(_started);
        dbgAssert(0 <= _interval);
        dbgAssert(_point <= now);

        if(!_repeatable)
        {
            _started = false;
        }

        if(_repeatable && _started)
        {
            if(now < _point)
            {
                _point += _interval;
                dbgAssert(_point >= now);
            }
            else
            {
                if(_interval > 0)
                {
                    PointRep next = _point + (now - _point)/_interval*_interval;

                    if(next > now)
                    {
                        _point = next;
                    }
                    else
                    {
                        _point = next + _interval;
                    }
                }
                else
                {
                    _point = now;
                }
                dbgAssert(_point >= now);
            }

            service.clocking().start(this);
        }

        if(_tickWire.connected())
        {
            _tickWire.in();
        }

        if(_tickRaisable)
        {
           _tickRaisable->raise();
        }
    }

}
