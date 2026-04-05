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
TEST(module_ppn_node_rdb, pql_eval_numseq)
{

    //numseq_max,
    EXPECT_EQ(380, (evalToInt<fun::numseq_max>(Value{List<Value>{Value{220}, Value{380}}})));

    //numseq_min,
    EXPECT_EQ(220, (evalToInt<fun::numseq_min>(Value{List<Value>{Value{220}, Value{380}}})));

    //numseq_avg,
    EXPECT_EQ(2, (evalToInt<fun::numseq_avg>(Value{List<Value>{Value{1},Value{1},Value{1},Value{1},Value{6}}})));

    //numseq_median,
    EXPECT_EQ(1, (evalToInt<fun::numseq_median>(Value{List<Value>{Value{1},Value{1},Value{1},Value{1},Value{6}}})));

    //numseq_sum,
    EXPECT_EQ(10, (evalToInt<fun::numseq_sum>(Value{List<Value>{Value{1},Value{1},Value{1},Value{1},Value{6}}})));

}
