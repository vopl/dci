/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "endian.hpp"
#include <cstddef>
#include <utility>

namespace dci::utils::endian
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::endian from, std::endian to, class T>
    T convert(T v) requires std::is_scalar_v<T>
    {
        if constexpr(from == to)
        {
            return v;
        }

        if constexpr(sizeof(T) == 1)
        {
            return v;
        }

        if constexpr(sizeof(T) == 2)
        {
            union
            {
                std::byte   raw[2];
                T           v;
            } u;

            u.v = v;

            std::swap(u.raw[0], u.raw[1]);

            return u.v;
        }

        if constexpr(sizeof(T) == 4)
        {
            union
            {
                std::byte   raw[4];
                T           v;
            } u;

            u.v = v;

            std::swap(u.raw[0], u.raw[3]);
            std::swap(u.raw[1], u.raw[2]);

            return u.v;
        }

        if constexpr(sizeof(T) == 8)
        {
            union
            {
                std::byte   raw[8];
                T           v;
            } u;

            u.v = v;

            std::swap(u.raw[0], u.raw[7]);
            std::swap(u.raw[1], u.raw[6]);
            std::swap(u.raw[2], u.raw[5]);
            std::swap(u.raw[3], u.raw[4]);

            return u.v;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> T l2b(T v) requires std::is_scalar_v<T> {return convert<std::endian::little, std::endian::big   >(v);}
    template <class T> T b2l(T v) requires std::is_scalar_v<T> {return convert<std::endian::big   , std::endian::little>(v);}
    template <class T> T n2b(T v) requires std::is_scalar_v<T> {return convert<std::endian::native, std::endian::big   >(v);}
    template <class T> T b2n(T v) requires std::is_scalar_v<T> {return convert<std::endian::big   , std::endian::native>(v);}
    template <class T> T l2n(T v) requires std::is_scalar_v<T> {return convert<std::endian::little, std::endian::native>(v);}
    template <class T> T n2l(T v) requires std::is_scalar_v<T> {return convert<std::endian::native, std::endian::little>(v);}

}
