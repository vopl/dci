// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <cstdint>
#include <vector>

namespace dci::crypto::rnd
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool API_DCI_CRYPTO generate(void* buf, std::size_t len);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C = std::uint8_t, class... CC>
    std::vector<C, CC...> generate(std::size_t len) requires std::is_standard_layout_v<C>;




    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    std::vector<C, CC...> generate(std::size_t len) requires std::is_standard_layout_v<C>
    {
        std::vector<C, CC...> res;
        res.resize(len);
        generate(res.data(), res.size());
        return res;
    }
}
