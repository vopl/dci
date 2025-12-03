// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pql_eval.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_node_rdb, pql_eval_seq)
{
    Value nums{List<Value>{{-1}, {0}, {1}, {2}}};
    List<Value> res;

    //seq_map,
    {
        List<Value> res = evalTo<fun::seq_map, List<Value>>(Node{fun::num_abs, List<Expression>{}}, nums);
        EXPECT_EQ(res, (List<Value>{{1}, {0}, {1}, {2}}));
    }

    //seq_grep,
    {
        List<Value> res = evalTo<fun::seq_grep, List<Value>>(Node{fun::num_lt, List<Expression>{Value{0}}}, nums);
        EXPECT_EQ(res, (List<Value>{{1}, {2}}));
    }

    //seq_reduce,
    {
        int res = evalTo<fun::seq_reduce, int>(Node{fun::num_add, List<Expression>{}}, nums);
        EXPECT_EQ(res, 2);
    }

    //seq_at,//element by index
    {
        int res = evalTo<fun::seq_at, int>(nums, Value{2});
        EXPECT_EQ(res, 1);
    }

    //seq_size,
    {
        std::size_t res = evalTo<fun::seq_size, std::size_t>(nums);
        EXPECT_EQ(res, 4u);
    }
}
