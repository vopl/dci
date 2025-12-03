// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
