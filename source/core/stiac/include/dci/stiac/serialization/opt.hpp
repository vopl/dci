/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/primitives.hpp>

namespace dci::stiac::serialization
{
    enum class OptStoreKind : uint8
    {
        null  = 0,
        value = 1,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, const Opt<T>& v)
    {
        if(!v)
        {
            ar << OptStoreKind::null;
            return;
        }

        ar << OptStoreKind::value;
        ar << *v;
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, Opt<T>&& v)
    {
        if(!v)
        {
            ar << OptStoreKind::null;
            return;
        }

        ar << OptStoreKind::value;
        ar << std::move(*v);
        v.reset();
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, Opt<T>& v)
    {
        OptStoreKind optStoreKind;
        ar >> optStoreKind;

        switch(optStoreKind)
        {
        case OptStoreKind::null:
            v.reset();
            return;

        case OptStoreKind::value:
            if(!v)
            {
                v.emplace();
            }
            ar >> *v;
            return;
        }

        ar.fail("malformed input stream (corrupted data)");
    }
}
