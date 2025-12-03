// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "timer.hpp"
#include "service.hpp"
#include <dci/cmt/functions.hpp>
#include <dci/utils/atScopeExit.hpp>

namespace dci::poll::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(Duration interval,
                     bool repeatable,
                     cmt::task::Owner* tickOwner,
                     cmt::Raisable* raisable)
        : _interval{interval.count()}
        , _repeatable{repeatable}
        , _tickOwner{(tickOwner ? tickOwner : &_localTickOwner)}
        , _raisable{raisable}
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
        return _tick->_wire.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::setTickOwner(cmt::task::Owner *tickOwner)
    {
        if (tickOwner != _tickOwner)
        {
            _tickOwner->stop(false);
        }
        _tickOwner = tickOwner ? tickOwner : &_localTickOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::resetTickOwner()
    {
        _localTickOwner.stop(false);
        _tickOwner = &_localTickOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::setRaisable(cmt::Raisable* raisable)
    {
        _raisable = raisable;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::resetRaisable()
    {
        _raisable = {};
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

        if(_tick->_wire.connected() && !_tick->_inProgress)
        {
            _tick->_inProgress = true;
            cmt::spawn() += _tickOwner * [tick{_tick}, cleaner{dci::utils::AtScopeExit{[tick=_tick]{tick->_inProgress=false;}}}]
            {
                tick->_wire.in();
            };
        }

        if(_raisable)
        {
           _raisable->raise();
        }
    }

}
