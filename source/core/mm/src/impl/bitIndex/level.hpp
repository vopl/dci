// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
