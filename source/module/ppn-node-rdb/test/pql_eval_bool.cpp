/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pql_eval.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_node_rdb, pql_eval_bool)
{
    //bool_not,
    EXPECT_EQ(false,    evalToBool<fun::bool_not>(Value{true}));

    //bool_and,
    EXPECT_EQ(false,    evalToBool<fun::bool_and>(Value{true}, Value{false}));

    //bool_or,
    EXPECT_EQ(true,     evalToBool<fun::bool_or>(Value{true}, Value{false}));

    //bool_xor,
    EXPECT_EQ(true,     evalToBool<fun::bool_xor>(Value{true}, Value{false}));
}
