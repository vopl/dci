/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/idl/iId.hpp>
#include <dci/idl/iSide.hpp>
using namespace dci::idl;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, tuid)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( IId{}                                                                                                     ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});

    hereThere2( IId{{0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}, ISide::primary}    ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    hereThere2( IId{{0x12,0x34,0x56,0x78,0x9a,0xbc,0xef,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}, ISide::opposite}   ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});

    hereThere2( IId{{0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}, ISide::opposite}   ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    hereThere2( IId{{0x12,0x34,0x56,0x78,0x9a,0xbc,0xef,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}, ISide::primary}    ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
}
