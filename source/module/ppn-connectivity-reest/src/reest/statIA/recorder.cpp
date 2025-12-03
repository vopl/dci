// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
