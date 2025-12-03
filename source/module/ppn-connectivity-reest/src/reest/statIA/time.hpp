// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::connectivity::reest::statIA
{
    using TimePoint = std::chrono::steady_clock::time_point;
    using TimeDuration = std::chrono::steady_clock::duration;

    TimePoint now();

    template<class Rep, class Period>
    real64 toSeconds(const std::chrono::duration<Rep, Period>& dur);







    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class Rep, class Period>
    real64 toSeconds(const std::chrono::duration<Rep, Period>& d)
    {
        return std::chrono::duration_cast<std::chrono::duration<real64>>(d).count();
    }
}
