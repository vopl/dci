/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "address.hpp"

#include <cstdint>
#include <type_traits>

namespace dci::mm::impl::bitIndex
{
    template <std::size_t order, std::size_t lineSize=64>
    class Level;

    template <std::size_t lineSize>
    class Level<0, lineSize>
    {
    public:
        static constexpr std::size_t _volume = lineSize*8;
        static constexpr std::size_t _subLevelsAmount = 0;
        static constexpr std::size_t _sizeofCounter = 0;
        static constexpr std::size_t _lineSize = lineSize;

    public:

        Address allocate();
        bool isAllocated(Address address) const;
        void deallocate(Address address);
        Address maxAllocatedAddress() const;
        std::size_t requiredAreaForAddress(Address address) const;

    private:
        using BitHolder = std::uint64_t;
        static constexpr std::size_t _bitHoldersAmount = lineSize / sizeof(BitHolder);

        BitHolder _bitHolders[_bitHoldersAmount];

        static_assert(sizeof(_bitHolders) == lineSize, "lineSize not aligned to 8 bytes?");
    };


    namespace
    {
        template <std::size_t value>
        using coveredUnsignedIntegral =
            std::conditional_t< (value < 1ULL << 8),
                std::uint8_t,
                std::conditional_t< (value < 1ULL << 16),
                    std::uint16_t,
                    std::conditional_t< (value < 1ULL << 32),
                        std::uint32_t,
                       std::uint64_t
                    >
                >
            >;
    }

    template <std::size_t order, std::size_t lineSize>
    class Level
    {
    public:

        Address allocate();
        bool isAllocated(Address address) const;
        void deallocate(Address address);
        Address maxAllocatedAddress() const;
        std::size_t requiredAreaForAddress(Address address) const;

    private:
        using SubLevel = Level<order-1, lineSize>;

        using Counter = coveredUnsignedIntegral<SubLevel::_volume>;

    public:
        static constexpr std::size_t _subLevelsAmount = (lineSize / sizeof(Counter));

    private:

        Counter _subLevelCounters[_subLevelsAmount];

        SubLevel _subLevels[_subLevelsAmount];

    public:
        static constexpr std::size_t _volume = _subLevelsAmount * SubLevel::_volume;
        static constexpr std::size_t _sizeofCounter = sizeof(Counter);
        static constexpr std::size_t _lineSize = lineSize;
    };

}
