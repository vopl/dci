// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "time.hpp"

namespace dci::module::ppn::connectivity::reest::statIA
{
    class Recorder
    {
    public:
        static constexpr TimeDuration _level0Period {std::chrono::seconds{60}};
        static constexpr uint32 _levelPeriodMult {5};
        static constexpr std::size_t _levels {8};

        using Counter = real32;

    public:
        Recorder(real32 initial = 0);
        ~Recorder();

        void fix(TimePoint t, real64 amount = 1);
        void dropNegatives();
        const std::array<Counter, _levels>& counters() const;

        real64 unfixedVolume(TimePoint t) const;

    private:
        TimePoint                       _lastFixMoment {};
        std::array<Counter, _levels>    _counters;
    };
}
