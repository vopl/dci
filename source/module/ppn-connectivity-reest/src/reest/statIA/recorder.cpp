/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "recorder.hpp"

namespace dci::module::ppn::connectivity::reest::statIA
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Recorder::Recorder(real32 initial)
    {
        _counters.fill(initial);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Recorder::~Recorder()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Recorder::fix(TimePoint t, real64 amount)
    {
        if(TimePoint{} == _lastFixMoment)
        {
            _lastFixMoment = t;
        }

        dbgAssert(t >= _lastFixMoment);
        const real64 volume = toSeconds(t - _lastFixMoment);
        real64 period = toSeconds(_level0Period);

        for(std::size_t i{0}; i<_counters.size(); ++i)
        {
            Counter& c = _counters[i];
            c = static_cast<Counter>((amount + static_cast<real64>(c) * period) / (period + volume));

            //std::cout<<"---- "<<this<<", "<<i<<", "<<(c)<<", "<<(c*period/1e9)<<std::endl;
            period *= _levelPeriodMult;
        }

        _lastFixMoment = t;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Recorder::dropNegatives()
    {
        for(Counter& c : _counters)
        {
            if(c < 0)
            {
                c = 0;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::array<Recorder::Counter, Recorder::_levels>& Recorder::counters() const
    {
        return _counters;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    real64 Recorder::unfixedVolume(TimePoint t) const
    {
        if(TimePoint{} == _lastFixMoment)
        {
            return 0;
        }

        dbgAssert(t >= _lastFixMoment);
        return std::chrono::duration_cast<std::chrono::duration<real64>>(t - _lastFixMoment).count();
    }
}
