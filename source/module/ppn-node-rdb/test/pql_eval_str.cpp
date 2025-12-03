// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pql_eval.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_node_rdb, pql_eval_str)
{
    //str_concat,
    EXPECT_EQ("abcd",   evalToString<fun::str_concat>(Value{String{"ab"}}, Value{String{"cd"}}));

    //str_find,
    EXPECT_EQ(1,        evalToInt<fun::str_find>(Value{String{"ab"}}, Value{String{"b"}}));

    //str_match,
    EXPECT_EQ(true,     evalToBool<fun::str_match>(Value{String{"ab"}}, Value{String{"ab"}}));

    //str_startsWith,
    EXPECT_EQ(true,     evalToBool<fun::str_startsWith>(Value{String{"ab"}}, Value{String{"a"}}));

    //str_endsWith,
    EXPECT_EQ(true,     evalToBool<fun::str_endsWith>(Value{String{"ab"}}, Value{String{"b"}}));

    //str_substr,
    EXPECT_EQ("bc",     evalToString<fun::str_substr>(Value{String{"abcd"}}, Value{1}, Value{2}));
}
