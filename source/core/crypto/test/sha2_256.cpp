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

using namespace dci::crypto;
using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(crypto, sha2_256)
{
    std::vector<uint8_t> digest(32);

    {
        Sha2_256 h;
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("3e0b4c2489cfc141a9bf4f8c99f69b4272ea144e46b939c44a5999b187258b55"));
    }

    {
        Sha2_256 h;
        h.add("The quick brown fox jumps over the lazy dog");
        h.finish(digest.data());
        EXPECT_EQ(digest, h2b("7d8abf3b707d084996aca9cb0b80e2f4d865154ed6c3bd67d2200dfb739c5e29"));
    }
}
