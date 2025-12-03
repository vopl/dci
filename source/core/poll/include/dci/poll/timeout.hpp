// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitableTimer.hpp"
#include <dci/cmt/event.hpp>

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Rep, class Period, class RaisableAndWaitable = dci::cmt::Event>
    auto timeout(std::chrono::duration<Rep, Period> interval);
}

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Rep, class Period, class RaisableAndWaitable>
    auto timeout(std::chrono::duration<Rep, Period> interval)
    {
        struct Timeout : WaitableTimer<RaisableAndWaitable>
        {
            Timeout(std::chrono::nanoseconds interval)
                : WaitableTimer<RaisableAndWaitable>{interval}
            {
                this->start();
            }
        };
        return Timeout{std::chrono::duration_cast<std::chrono::nanoseconds>(interval)};
    }
}
