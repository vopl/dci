/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <climits>

namespace dci::utils::bits
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Integral>
    constexpr std::size_t bitsof(Integral) noexcept requires std::is_integral<Integral>::value
    {
        return sizeof(Integral)*CHAR_BIT;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class Integral> constexpr std::size_t ctz(Integral x) noexcept requires ( sizeof(Integral) <= sizeof(int) )
        {
            return static_cast<std::size_t>(__builtin_ctz(std::make_unsigned_t<Integral>(x)));
        }

        template <class Integral> constexpr std::size_t ctz(Integral x) noexcept requires (sizeof(Integral) > sizeof(int) && sizeof(Integral) <= sizeof(long))
        {
            return static_cast<std::size_t>(__builtin_ctzl(std::make_unsigned_t<Integral>(x)));
        }

        template <class Integral> constexpr std::size_t ctz(Integral x) noexcept requires (sizeof(Integral) > sizeof(long) && sizeof(Integral) <= sizeof(long long))
        {
            return static_cast<std::size_t>(__builtin_ctzll(std::make_unsigned_t<Integral>(x)));
        }

        template <class Integral> constexpr std::size_t clz(Integral x) noexcept requires (sizeof(Integral) <= sizeof(int))
        {
            return static_cast<std::size_t>(static_cast<std::size_t>(__builtin_clz(std::make_unsigned_t<Integral>(x))) + bitsof<Integral>() - bitsof<int>());
        }

        template <class Integral> constexpr std::size_t clz(Integral x) noexcept requires (sizeof(Integral) > sizeof(int) && sizeof(Integral) <= sizeof(long))
        {
            return static_cast<std::size_t>(__builtin_clzl(std::make_unsigned_t<Integral>(x)));
        }

        template <class Integral> constexpr std::size_t clz(Integral x) noexcept requires (sizeof(Integral) > sizeof(long) && sizeof(Integral) <= sizeof(long long))
        {
            return static_cast<std::size_t>(__builtin_clzll(std::make_unsigned_t<Integral>(x)));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Integral>
    constexpr std::size_t count0Least(Integral x) noexcept requires std::is_integral<Integral>::value
    {
        if(0 == x)
        {
            return bitsof(x);
        }

        return details::ctz(x);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Integral>
    constexpr std::size_t count0Most(Integral x) noexcept requires std::is_integral<Integral>::value
    {
        if(0 == x)
        {
            return bitsof(x);
        }

        return details::clz(x);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Integral>
    constexpr std::size_t count1Least(Integral x) noexcept requires std::is_integral<Integral>::value
    {
        return count0Least(Integral(~x));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Integral>
    constexpr std::size_t count1Most(Integral x) noexcept requires std::is_integral<Integral>::value
    {
        return count0Most(Integral(~x));
    }
}
