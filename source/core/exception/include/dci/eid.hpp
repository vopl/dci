// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <array>
#include <cstdint>

namespace dci
{
    struct Eid : std::array<std::uint8_t, 16> {};
}
