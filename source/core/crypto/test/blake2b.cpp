/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/crypto.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/b2h.hpp>

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, blake2b)
{
    std::vector<uint8_t> digest(64);

    {
        Blake2b h;
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("87a6207f241095306c6cdf5852252d2719f274041e857416a8682e717ff145912de50113faee85353198464439e40bb409a386b541847b555df607a1efb92eec"));
    }

    {
        Blake2b h;
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("8ada4ddbdddf394e78d772646e82711b6163a4f17acb41d85990b07c33b363378f4210fca72a4ebce1dc0992e6f341bc45318fde77eb3740b53119c4cd6d9a81"));
    }
}
