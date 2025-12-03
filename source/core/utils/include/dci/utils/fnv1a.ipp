// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include <cstdint>

namespace dci::utils
{
    template <class Char>
    constexpr std::uint64_t fnv1a(const Char* material, std::size_t len)
    {
        std::uint64_t result = 0xcbf29ce484222325;

        for(std::size_t i{0}; i<len; ++i)
        {
            result ^= static_cast<std::uint64_t>(material[i]);
            result *= 0x100000001b3;
        }

        return result;
    }

    template <class Container>
    constexpr std::uint64_t fnv1a(const Container& material)
    {
        return fnv1a(material.data(), material.size());
    }

    template <class LiteralChar, std::size_t N>
    constexpr std::uint64_t fnv1a(const LiteralChar (&material)[N])
    {
        static_assert(N > 0);
        return fnv1a(&material[0], N-1);
    }
}
