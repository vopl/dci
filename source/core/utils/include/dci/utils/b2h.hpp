// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <cstring>
#include <string>
#include <cstdint>
#include <vector>
#include <array>
#include "hexEndian.hpp"

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS void b2h(const void* b, std::size_t bsize, void* h, HexEndian he = HexEndian::little);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS std::string b2h(const void* b, std::size_t bsize, HexEndian he = HexEndian::little);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C = std::uint8_t, class... CC>
    std::string b2h(const std::vector<C, CC...>& b, HexEndian he = HexEndian::little);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C = std::uint8_t, std::size_t N>
    std::string b2h(const std::array<C, N>& b, HexEndian he = HexEndian::little);
}

#include "b2h.ipp"
