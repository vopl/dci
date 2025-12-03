// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pql_eval.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_node_rdb, pql_eval_num)
{

    //num_eq,
    EXPECT_FALSE(evalToBool<fun::num_eq>(Value{220}, Value{380}));
    EXPECT_TRUE (evalToBool<fun::num_eq>(Value{1.23}, Value{1.24}, Value{0.02}));

    //num_ne,
    EXPECT_TRUE (evalToBool<fun::num_ne>(Value{220}, Value{380}));
    EXPECT_FALSE(evalToBool<fun::num_ne>(Value{1.23}, Value{1.24}, Value{0.02}));

    //num_gt,
    EXPECT_FALSE(evalToBool<fun::num_gt>(Value{220}, Value{380}));
    EXPECT_TRUE (evalToBool<fun::num_gt>(Value{1.25}, Value{1.24}));

    //num_lt,
    EXPECT_TRUE (evalToBool<fun::num_lt>(Value{220}, Value{380}));
    EXPECT_FALSE(evalToBool<fun::num_lt>(Value{1.25}, Value{1.24}));

    //num_ge,
    EXPECT_FALSE(evalToBool<fun::num_ge>(Value{220}, Value{380}));
    EXPECT_TRUE (evalToBool<fun::num_ge>(Value{1.25}, Value{1.24}));
    EXPECT_TRUE (evalToBool<fun::num_ge>(Value{1.24}, Value{1.24}));

    //num_le,
    EXPECT_TRUE (evalToBool<fun::num_le>(Value{220}, Value{380}));
    EXPECT_FALSE(evalToBool<fun::num_le>(Value{1.25}, Value{1.24}));
    EXPECT_TRUE (evalToBool<fun::num_le>(Value{1.24}, Value{1.24}));

    //num_between
    EXPECT_TRUE (evalToBool<fun::num_between>(Value{1.24}, Value{1.24}, Value{1.24}));
    EXPECT_TRUE (evalToBool<fun::num_between>(Value{2}, Value{1}, Value{3}));
    EXPECT_FALSE(evalToBool<fun::num_between>(Value{0}, Value{1}, Value{3}));
    EXPECT_FALSE(evalToBool<fun::num_between>(Value{4}, Value{1}, Value{3}));

    //num_neg,
    EXPECT_EQ       (-1,    evalToInt<fun::num_neg>(Value{1}));
    EXPECT_DOUBLE_EQ(-1.2,  evalToReal<fun::num_neg>(Value{1.2}));
    EXPECT_EQ       (-1,    (evalTo<fun::num_neg, int64>(Value{int64{1}})));

    //num_add,
    EXPECT_EQ       (220,           evalToInt<fun::num_add>(Value{100}, Value{120}));

    //num_sub,
    EXPECT_EQ       (-20,           evalToInt<fun::num_sub>(Value{100}, Value{120}));

    //num_div,
    EXPECT_EQ       (0,             evalToInt<fun::num_div>(Value{100}, Value{120}));
    EXPECT_EQ       (Null{},        evalToNull<fun::num_div>(Value{100}, Value{0}));

    //num_mul,
    EXPECT_EQ       (2000,          evalToInt<fun::num_mul>(Value{100}, Value{20}));

    //num_mod,
    EXPECT_EQ       (5,             evalToInt<fun::num_mod>(Value{100}, Value{19}));
    EXPECT_EQ       (Null{},        evalToNull<fun::num_mod>(Value{100}, Value{0}));

    //num_ceil,
    EXPECT_DOUBLE_EQ(2.0,           evalToReal<fun::num_ceil>(Value{1.2}));

    //num_floor,
    EXPECT_DOUBLE_EQ(1.0,           evalToReal<fun::num_floor>(Value{1.2}));

    //num_trunc,
    EXPECT_DOUBLE_EQ(1.0,           evalToReal<fun::num_trunc>(Value{1.2}));

    //num_round,
    EXPECT_DOUBLE_EQ(1.0,           evalToReal<fun::num_round>(Value{1.2}));

    //num_abs,
    EXPECT_DOUBLE_EQ(1.2,           evalToReal<fun::num_abs>(Value{-1.2}));

    //num_log,
    EXPECT_DOUBLE_EQ(log(1.2),      evalToReal<fun::num_log>(Value{1.2}));

    //num_log10,
    EXPECT_DOUBLE_EQ(log10(1.2),    evalToReal<fun::num_log10>(Value{1.2}));

    //num_log2,
    EXPECT_DOUBLE_EQ(log2(1.2),     evalToReal<fun::num_log2>(Value{1.2}));

    //num_exp,
    EXPECT_DOUBLE_EQ(exp(1.2),      evalToReal<fun::num_exp>(Value{1.2}));

    //num_exp10,
    EXPECT_DOUBLE_EQ(pow(10.0, 1.2),    evalToReal<fun::num_exp10>(Value{1.2}));

    //num_exp2,
    EXPECT_DOUBLE_EQ(exp2(1.2),     evalToReal<fun::num_exp2>(Value{1.2}));

    //num_pow,
    EXPECT_DOUBLE_EQ(pow(1.2, 3.4), evalToReal<fun::num_pow>(Value{1.2}, Value{3.4}));

    //num_sqrt,
    EXPECT_DOUBLE_EQ(sqrt(1.2),     evalToReal<fun::num_sqrt>(Value{1.2}));

    //num_cbrt,
    EXPECT_DOUBLE_EQ(cbrt(1.2),     evalToReal<fun::num_cbrt>(Value{1.2}));

    //num_hypot,
    EXPECT_DOUBLE_EQ(hypot(1.2, 3.4),evalToReal<fun::num_hypot>(Value{1.2}, Value{3.4}));

    //num_sin,
    EXPECT_DOUBLE_EQ(sin(1.2),      evalToReal<fun::num_sin>(Value{1.2}));

    //num_cos,
    EXPECT_DOUBLE_EQ(cos(1.2),      evalToReal<fun::num_cos>(Value{1.2}));

    //num_tan,
    EXPECT_DOUBLE_EQ(tan(1.2),      evalToReal<fun::num_tan>(Value{1.2}));

    //num_asin,
    EXPECT_DOUBLE_EQ(asin(0.2),     evalToReal<fun::num_asin>(Value{0.2}));

    //num_acos,
    EXPECT_DOUBLE_EQ(acos(0.2),     evalToReal<fun::num_acos>(Value{0.2}));

    //num_atan,
    EXPECT_DOUBLE_EQ(atan(0.2),     evalToReal<fun::num_atan>(Value{0.2}));

    //num_atan2,
    EXPECT_DOUBLE_EQ(atan2(0.2, 0.3),evalToReal<fun::num_atan2>(Value{0.2}, Value{0.3}));

    //num_toIdTip,
//    EXPECT_EQ((id128{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}),           (evalTo<fun::num_toId128Tip, id128>(0)));
//    EXPECT_EQ((id128{0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}),           (evalTo<fun::num_toId128Tip, id128>(1)));
//    EXPECT_EQ((id128{0xff,0x7f,0x35,0x2f,0x81,0x9f,0x4e,0xb1,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00}),           (evalTo<fun::num_toId128Tip, id128>(1.234567890123456e20)));
//    EXPECT_EQ((id128{0xff,0xff,0xff,0xff,0xff,0xff,0xaf,0x98,0x41,0x1c,0x4c,0x9b,0x1d,0x00,0x00,0x00}),           (evalTo<fun::num_toId128Tip, id128>(2.345678901234567e30)));
//    EXPECT_EQ((id128{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}),           (evalTo<fun::num_toId128Tip, id128>(1e39)));

//    EXPECT_EQ((id256{0xff,0xff,0xff,0xff,0xff,0xff,0xaf,0x98,0x41,0x1c,0x4c,0x9b,0x1d,0x00,0x00,0x00}),           (evalTo<fun::num_toId256Tip, id256>(2.345678901234567e30)));
//    EXPECT_EQ((id512{0xff,0xff,0xff,0xff,0xff,0xff,0xaf,0x98,0x41,0x1c,0x4c,0x9b,0x1d,0x00,0x00,0x00}),           (evalTo<fun::num_toId512Tip, id512>(2.345678901234567e30)));
}
