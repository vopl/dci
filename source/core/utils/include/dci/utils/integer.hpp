// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
