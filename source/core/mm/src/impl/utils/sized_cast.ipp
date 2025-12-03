// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "sized_cast.hpp"
#include <bit>

namespace dci::mm::impl::utils
{
    template <class To, class From>
    constexpr To sized_cast(const From& from) noexcept requires (sizeof(From) == sizeof(To) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>)
    {
        return std::bit_cast<To>(from);
    }
}
