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
#include "../smallIntegral.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, const Set<T>& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));

        for(const T& e : v)
        {
            ar << e;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, Set<T>&& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));

        while(!v.empty())
        {
            auto node = v.extract(v.begin());
            ar << std::move(node.value());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, Set<T>& v)
    {
        uint32 size;
        ar >> smallIntegral(size);

        if(size * 1 > ar.size())
        {
            ar.fail("malformed input stream (low data)");
        }

        v.clear();

        T e;
        while(size)
        {
            ar >> e;
            v.emplace_hint(v.end(), std::move(e));
            size--;
        }
    }
}
