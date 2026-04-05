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
#include "fixEndian.hpp"
#include <type_traits>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept SmallIntegralUnsigned = T::_isSmallIntegral && T::_isUnsigned;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <SmallIntegralUnsigned T>
    void save(auto& ar, const T& v)
    {
        static_assert(sizeof(v)<=8);

        if constexpr(1 == sizeof(v))
        {
            ar.write(&v, 1);
            return;
        }

        if constexpr(2 == sizeof(v))
        {
            typename T::V vFixed = fixEndian(v._v);
            ar.write(&vFixed, 2);
            return;
        }

        if constexpr(4 <= sizeof(v))
        {
            static constexpr std::size_t bufSize = sizeof(v)+1;

            uint8 buf[bufSize];

            typename T::V u = v._v;

            for(std::size_t i(0); i<bufSize-1; ++i)
            {
                buf[i] = (u & 0x7f) | 0x80;
                u >>= 7;

                if(!u)
                {
                    buf[i] ^= 0x80;
                    ar.write(&buf[0], static_cast<uint32>(i+1));
                    return;
                }

                u -= typename T::V{1};
            }

            buf[bufSize-1] = u & 0xff;
            ar.write(&buf[0], bufSize);
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <SmallIntegralUnsigned T>
    void load(auto& ar, T& v)
    {
        static_assert(sizeof(v)<=8);

        if constexpr(1 == sizeof(v))
        {
            return ar.read(&v, 1);
        }

        if constexpr(2 == sizeof(v))
        {
            typename T::V u;
            ar.read(&u, 2);
            v._v = fixEndian(u);
            return;
        }

        static constexpr std::size_t bufSize = sizeof(v)+1;

        using U = typename T::V;
        U u = 0;

        uint8 byte;
        std::size_t i(0);
        for(; i<bufSize-1; ++i)
        {
            ar.read(&byte, 1);

            u += U(byte & 0xff) << (i*7);

            if(!(byte & 0x80))
            {
                v._v = u;
                return;
            }
        }

        ar.read(&byte, 1);
        u += U(byte) << ((bufSize-1)*7);

        v._v = u;
    }
}
