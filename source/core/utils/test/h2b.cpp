/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/endian.hpp>

using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, h2b)
{
    std::uint32_t bin;

    EXPECT_TRUE(h2b("abcdef00", 8, &bin, HexEndian::little));
    EXPECT_EQ(endian::l2n(bin), std::uint32_t{0x00fedcba});

    EXPECT_TRUE(h2b("abcdef00", 8, &bin, HexEndian::middle));
    EXPECT_EQ(endian::l2n(bin), std::uint32_t{0x00efcdab});

    EXPECT_TRUE(h2b("abcdef00", 8, &bin, HexEndian::big));
    EXPECT_EQ(endian::l2n(bin), std::uint32_t{0xabcdef00});
}
