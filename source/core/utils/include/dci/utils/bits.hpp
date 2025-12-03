// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include <type_traits>

namespace dci::utils::bits
{
    template <class Integral> constexpr std::size_t bitsof(Integral = Integral()) noexcept requires std::is_integral<Integral>::value;

    template <class Integral> constexpr std::size_t count0Least(Integral x) noexcept requires std::is_integral<Integral>::value;
    template <class Integral> constexpr std::size_t count0Most (Integral x) noexcept requires std::is_integral<Integral>::value;
    template <class Integral> constexpr std::size_t count1Least(Integral x) noexcept requires std::is_integral<Integral>::value;
    template <class Integral> constexpr std::size_t count1Most (Integral x) noexcept requires std::is_integral<Integral>::value;
}

#include "bits.ipp"
