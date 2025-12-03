// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <chrono>

namespace dci::poll::impl::clocking
{
    using Clock         = std::chrono::steady_clock;
    using Duration      = Clock::duration;
    using DurationRep   = Duration::rep;
    using Point         = Clock::time_point;
    using PointRep      = Point::rep;
}
