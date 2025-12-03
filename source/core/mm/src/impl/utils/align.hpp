// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>

namespace dci::mm::impl::utils
{
    ////////////////////////////////////////////////////////////////
    static constexpr std::size_t alignDown(std::size_t size, std::size_t alignment)
    {
        return (size / alignment * alignment);
    }

    ////////////////////////////////////////////////////////////////
    static constexpr std::size_t alignUp(std::size_t size, std::size_t alignment)
    {
        return alignDown(size + alignment - 1, alignment);
    }
}
