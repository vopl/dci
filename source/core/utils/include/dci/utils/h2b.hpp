// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <cstring>
#include <cstdint>
#include <vector>
#include <array>
#include <type_traits>
#include "hexEndian.hpp"

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS bool h2b(const void* h, std::size_t hsize, void* b, HexEndian he = HexEndian::little);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    bool h2b(const void* h, std::size_t hsize, std::vector<C, CC...>& b, HexEndian he = HexEndian::little) requires std::is_standard_layout_v<C>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    bool h2b(const char* csz, std::vector<C, CC...>& b, HexEndian he = HexEndian::little) requires std::is_standard_layout_v<C>;
 \
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C = std::uint8_t, class... CC>
    std::vector<C, CC...> h2b(const void* h, std::size_t hsize, HexEndian he = HexEndian::little) requires std::is_standard_layout_v<C>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C = std::uint8_t, class... CC>
    std::vector<C, CC...> h2b(const char* csz, HexEndian he = HexEndian::little) requires std::is_standard_layout_v<C>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C = std::uint8_t, std::size_t N>
    bool h2b(const char* csz, std::array<C, N>& buf, HexEndian he = HexEndian::little) requires std::is_standard_layout_v<C>;
}

#include "h2b.ipp"
