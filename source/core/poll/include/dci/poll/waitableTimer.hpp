// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/poll/timer.hpp>
#include <dci/cmt/notifier.hpp>

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class RaisableAndWaitable = dci::cmt::Notifier>
    class WaitableTimer
        : public Timer
    {
    public:
        WaitableTimer(std::chrono::nanoseconds interval);
        WaitableTimer(std::chrono::nanoseconds interval, bool repeatable);

        RaisableAndWaitable& raisableAndWaitable();
        dci::cmt::Waitable& waitable();
        void wait();

    private:
        RaisableAndWaitable _raisableAndWaitable;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class RaisableAndWaitable>
    WaitableTimer<RaisableAndWaitable>::WaitableTimer(std::chrono::nanoseconds interval)
        : Timer{interval, &_raisableAndWaitable}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class RaisableAndWaitable>
    WaitableTimer<RaisableAndWaitable>::WaitableTimer(std::chrono::nanoseconds interval, bool repeatable)
        : Timer{interval, repeatable, &_raisableAndWaitable}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class RaisableAndWaitable>
    RaisableAndWaitable& WaitableTimer<RaisableAndWaitable>::raisableAndWaitable()
    {
        return _raisableAndWaitable;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class RaisableAndWaitable>
    dci::cmt::Waitable& WaitableTimer<RaisableAndWaitable>::waitable()
    {
        return _raisableAndWaitable;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class RaisableAndWaitable>
    void WaitableTimer<RaisableAndWaitable>::wait()
    {
        _raisableAndWaitable.wait();
    }
}
