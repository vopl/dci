/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "bits.hpp"
#include <type_traits>
#include <cstdint>
#include <climits>

namespace dci::utils::integer
{
    namespace details
    {
        template <std::size_t bits, class i8, class i16, class i32, class i64, class bad=void> using selectByBits =
            std::conditional_t< (bits <= 8),
                i8,
                std::conditional_t< (bits <= 16),
                    i16,
                    std::conditional_t< (bits <= 32),
                        i32,
                        std::conditional_t< (bits <= 64),
                            i64,
                            bad
                        >
                    >
                >
            >;

        template <std::size_t count, class i8, class i16, class i32, class i64, class bad=void> using selectByCount =
            std::conditional_t< (count <= 0xff),
                i8,
                std::conditional_t< (count <= 0xffff),
                    i16,
                    std::conditional_t< (count <= 0xffffffff),
                        i32,
                        std::conditional_t< (count <= 0xffffffffffffffff),
                            i64,
                            bad
                        >
                    >
                >
            >;
    }

//    template <class T> using intCover       = details::selectCover<bitsof(T), std::int8_t,        std::int16_t,        std::int32_t,        std::int64_t>;
//    template <class T> using uintCover      = details::selectCover<bitsof(T), std::uint8_t,       std::uint16_t,       std::uint32_t,       std::uint64_t>;
//    template <class T> using fastintCover   = details::selectCover<bitsof(T), std::int_fast8_t,   std::int_fast16_t,   std::int_fast32_t,   std::int_fast64_t>;
//    template <class T> using fastuintCover  = details::selectCover<bitsof(T), std::uint_fast8_t,  std::uint_fast16_t,  std::uint_fast32_t,  std::uint_fast64_t>;
//    template <class T> using leastintCover  = details::selectCover<bitsof(T), std::int_least8_t,  std::int_least16_t,  std::int_least32_t,  std::int_least64_t>;
//    template <class T> using leastuintCover = details::selectCover<bitsof(T), std::uint_least8_t, std::uint_least16_t, std::uint_least32_t, std::uint_least64_t>;

    template <std::size_t bits> using intCover       = details::selectByBits<bits, std::int8_t,        std::int16_t,        std::int32_t,        std::int64_t>;
    template <std::size_t bits> using uintCover      = details::selectByBits<bits, std::uint8_t,       std::uint16_t,       std::uint32_t,       std::uint64_t>;
    template <std::size_t bits> using fastintCover   = details::selectByBits<bits, std::int_fast8_t,   std::int_fast16_t,   std::int_fast32_t,   std::int_fast64_t>;
    template <std::size_t bits> using fastuintCover  = details::selectByBits<bits, std::uint_fast8_t,  std::uint_fast16_t,  std::uint_fast32_t,  std::uint_fast64_t>;
    template <std::size_t bits> using leastintCover  = details::selectByBits<bits, std::int_least8_t,  std::int_least16_t,  std::int_least32_t,  std::int_least64_t>;
    template <std::size_t bits> using leastuintCover = details::selectByBits<bits, std::uint_least8_t, std::uint_least16_t, std::uint_least32_t, std::uint_least64_t>;

    template <std::size_t count> using intCounter       = details::selectByCount<count, std::int8_t,        std::int16_t,        std::int32_t,        std::int64_t>;
    template <std::size_t count> using uintCounter      = details::selectByCount<count, std::uint8_t,       std::uint16_t,       std::uint32_t,       std::uint64_t>;
    template <std::size_t count> using fastintCounter   = details::selectByCount<count, std::int_fast8_t,   std::int_fast16_t,   std::int_fast32_t,   std::int_fast64_t>;
    template <std::size_t count> using fastuintCounter  = details::selectByCount<count, std::uint_fast8_t,  std::uint_fast16_t,  std::uint_fast32_t,  std::uint_fast64_t>;
    template <std::size_t count> using leastintCounter  = details::selectByCount<count, std::int_least8_t,  std::int_least16_t,  std::int_least32_t,  std::int_least64_t>;
    template <std::size_t count> using leastuintCounter = details::selectByCount<count, std::uint_least8_t, std::uint_least16_t, std::uint_least32_t, std::uint_least64_t>;
}
