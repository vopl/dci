// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "state.hpp"

namespace dci::module::www::http::inputSlicer::state
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Headers::Current::reset()
    {
        _key.reset();
        _value.reset();
        _kind = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Headers::Сonveyor::canDetachSome() const
    {
        if(_allowLastValueContinue)
            return _tail.size() > 1;

        return !_tail.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    primitives::List<api::http::Header> Headers::Сonveyor::detachSome()
    {
        if(_allowLastValueContinue)
        {
            if(_tail.size() < 2)
                return {};

            primitives::List<api::http::Header> toDetach;
            toDetach.swap(_tail);

            _tail.emplace_back(std::move(toDetach.front()));
            toDetach.pop_front();

            return std::exchange(toDetach, {});
        }

        return std::exchange(_tail, {});
    }
}
