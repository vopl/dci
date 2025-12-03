// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
