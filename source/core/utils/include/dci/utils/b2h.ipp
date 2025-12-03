// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "b2h.hpp"
//#include <dci/utils/dbg.hpp>

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    std::string b2h(const std::vector<C, CC...>& b, HexEndian he)
    {
        return b2h(b.data(), b.size()*sizeof(C), he);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, std::size_t N>
    std::string b2h(const std::array<C, N>& b, HexEndian he)
    {
        return b2h(b.data(), b.size()*sizeof(C), he);
    }
}
