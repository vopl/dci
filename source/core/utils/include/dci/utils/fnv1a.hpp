// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include <cstdint>

namespace dci::utils
{
    template <class Char>
    constexpr std::uint64_t fnv1a(const Char* material, std::size_t len);

    template <class Container>
    constexpr std::uint64_t fnv1a(const Container& material);

    template <class LiteralChar, std::size_t N>
    constexpr std::uint64_t fnv1a(const LiteralChar (&material)[N]);
}

#include "fnv1a.ipp"
