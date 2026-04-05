/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/sbs/wire.hpp>
#include <dci/mm/heap/allocable.hpp>
#include <dci/primitives.hpp>

#include <type_traits>
#include <utility>
#include <cstdint>

namespace dci::idl::contract
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class UseCounter
    {
    public:
        UseCounter();
        ~UseCounter();

        void incStrong();
        uint32 decStrong();
        uint32 valStrong() const;

        void incWeak();
        uint32 decWeak();
        uint32 valWeak() const;

        sbs::Signal<void, bool> involvedChanged();

        void dropSbs();

    private:
        uint32                  _valueStrong{};
        uint32                  _valueWeak{};
        sbs::Wire<void, bool>   _sbsWire;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline UseCounter::UseCounter()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline UseCounter::~UseCounter()
    {
        dbgAssert(!_valueStrong);
        dbgAssert(!_valueWeak);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void UseCounter::incStrong()
    {
        dbgAssert(_valueStrong < _valueStrong+1);
        ++_valueStrong;

        if(1 == _valueStrong)
        {
            _sbsWire.in(true);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline uint32 UseCounter::decStrong()
    {
        dbgAssert(_valueStrong > 0);

        uint32 res = --_valueStrong;

        if(!res)
        {
            _sbsWire.in(false);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline uint32 UseCounter::valStrong() const
    {
        return _valueStrong;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void UseCounter::incWeak()
    {
        dbgAssert(_valueWeak < _valueWeak+1);
        ++_valueWeak;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline uint32 UseCounter::decWeak()
    {
        dbgAssert(_valueWeak > 0);
        return --_valueWeak;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline uint32 UseCounter::valWeak() const
    {
        return _valueWeak;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline sbs::Signal<void, bool> UseCounter::involvedChanged()
    {
        return _sbsWire.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void UseCounter::dropSbs()
    {
        _sbsWire.disconnectAll();
    }
}
