/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "h2b.hpp"
#include <dci/utils/dbg.hpp>

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    bool h2b(const void* h, std::size_t hsize, std::vector<C, CC...>& b, HexEndian he) requires std::is_standard_layout_v<C>
    {
        b.resize((hsize/2) + (hsize&1));
        return h2b(h, hsize, b.data(), he);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    bool h2b(const char* csz, std::vector<C, CC...>& b, HexEndian he) requires std::is_standard_layout_v<C>
    {
        std::size_t hsize = strlen(csz);
        b.resize((hsize/2) + (hsize&1));
        return h2b(csz, hsize, b.data(), he);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    std::vector<C, CC...> h2b(const void* h, std::size_t hsize, HexEndian he) requires std::is_standard_layout_v<C>
    {
        std::vector<C, CC...> res;
        h2b(h, hsize, res, he);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, class... CC>
    std::vector<C, CC...> h2b(const char* csz, HexEndian he) requires std::is_standard_layout_v<C>
    {
        std::vector<C, CC...> res;
        h2b(csz, res, he);
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class C, std::size_t N>
    bool h2b(const char* csz, std::array<C, N>& buf, HexEndian he) requires std::is_standard_layout_v<C>
    {
        std::size_t hsize = strlen(csz);
        std::size_t bsize = (hsize/2) + (hsize&1);

        std::size_t bufSize = N * sizeof(C);
        if(bsize > bufSize)
        {
            return false;
        }

        if(!h2b(csz, hsize, buf.data(), he))
        {
            return false;
        }

        if(bsize < bufSize)
        {
            std::uint8_t* buf8 = static_cast<std::uint8_t *>(static_cast<void *>(buf.data()));
            memset(buf8+bsize, 0, bufSize - bsize);
        }

        return true;
    }

}
