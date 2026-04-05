/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
