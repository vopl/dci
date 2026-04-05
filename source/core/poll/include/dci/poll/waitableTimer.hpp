/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
