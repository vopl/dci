// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pql_eval.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_ppn_node_rdb, pql_eval_cmn)
{
    //cmn_eqType,
    EXPECT_TRUE (evalToBool<fun::cmn_eqType>(   vNull,      vNull       ));
    EXPECT_TRUE (evalToBool<fun::cmn_eqType>(   vInt8,      vInt8       ));
    EXPECT_TRUE (evalToBool<fun::cmn_eqType>(   vBool,      vBool_1     ));

    EXPECT_FALSE(evalToBool<fun::cmn_eqType>(   vNull,      vBool_1     ));
    EXPECT_FALSE(evalToBool<fun::cmn_eqType>(   vUint8,     vBool_1     ));
    EXPECT_FALSE(evalToBool<fun::cmn_eqType>(   vReal32,    vUint8      ));

    //cmn_eq,
    EXPECT_TRUE (evalToBool<fun::cmn_eq>(       vNull,      vNull       ));
    EXPECT_TRUE (evalToBool<fun::cmn_eq>(       vBool_1,    vBool_1     ));
    EXPECT_TRUE (evalToBool<fun::cmn_eq>(       vString_1,  vString_1   ));
    EXPECT_TRUE (evalToBool<fun::cmn_eq>(       vUint8_1,   vUint8_1    ));

    EXPECT_FALSE(evalToBool<fun::cmn_eq>(       vNull,      vBool       ));
    EXPECT_FALSE(evalToBool<fun::cmn_eq>(       vBool,      vBool_1     ));
    EXPECT_FALSE(evalToBool<fun::cmn_eq>(       vString,    vString_1   ));
    EXPECT_FALSE(evalToBool<fun::cmn_eq>(       vUint8,     vUint8_1    ));

    //cmn_ne,
    EXPECT_FALSE(evalToBool<fun::cmn_ne>(       vNull,      vNull       ));
    EXPECT_FALSE(evalToBool<fun::cmn_ne>(       vBool_1,    vBool_1     ));
    EXPECT_FALSE(evalToBool<fun::cmn_ne>(       vString_1,  vString_1   ));
    EXPECT_FALSE(evalToBool<fun::cmn_ne>(       vUint8_1,   vUint8_1    ));

    EXPECT_TRUE (evalToBool<fun::cmn_ne>(       vNull,      vBool       ));
    EXPECT_TRUE (evalToBool<fun::cmn_ne>(       vBool,      vBool_1     ));
    EXPECT_TRUE (evalToBool<fun::cmn_ne>(       vString,    vString_1   ));
    EXPECT_TRUE (evalToBool<fun::cmn_ne>(       vUint8,     vUint8_1    ));

    //cmn_gt,
    EXPECT_FALSE(evalToBool<fun::cmn_gt>(       vNull,      vNull       ));
    EXPECT_TRUE (evalToBool<fun::cmn_gt>(       vBool_1,    vBool       ));
    EXPECT_FALSE(evalToBool<fun::cmn_gt>(       vString,    vString_1   ));
    EXPECT_FALSE(evalToBool<fun::cmn_gt>(       vUint8_1,   vId128_1    ));
    EXPECT_TRUE (evalToBool<fun::cmn_gt>(       vId128_1,   vUint8_1    ));

//    //cmn_lt,
    EXPECT_FALSE(evalToBool<fun::cmn_lt>(       vNull,      vNull       ));
    EXPECT_FALSE(evalToBool<fun::cmn_lt>(       vBool_1,    vBool       ));
    EXPECT_TRUE (evalToBool<fun::cmn_lt>(       vString,    vString_1   ));
    EXPECT_TRUE (evalToBool<fun::cmn_lt>(       vUint8_1,   vId128_1    ));
    EXPECT_FALSE(evalToBool<fun::cmn_lt>(       vId128_1,   vUint8_1    ));

    //cmn_ge,
    EXPECT_TRUE (evalToBool<fun::cmn_ge>(       vNull,      vNull       ));
    EXPECT_TRUE (evalToBool<fun::cmn_ge>(       vBool_1,    vBool       ));
    EXPECT_FALSE(evalToBool<fun::cmn_ge>(       vString,    vString_1   ));
    EXPECT_FALSE(evalToBool<fun::cmn_ge>(       vUint8_1,   vId128_1    ));
    EXPECT_TRUE (evalToBool<fun::cmn_ge>(       vId128_1,   vUint8_1    ));
    EXPECT_TRUE (evalToBool<fun::cmn_ge>(       vId128_1,   vId128_1    ));

    //cmn_le,
    EXPECT_TRUE (evalToBool<fun::cmn_le>(       vNull,      vNull       ));
    EXPECT_FALSE(evalToBool<fun::cmn_le>(       vBool_1,    vBool       ));
    EXPECT_TRUE (evalToBool<fun::cmn_le>(       vString,    vString_1   ));
    EXPECT_TRUE (evalToBool<fun::cmn_le>(       vUint8_1,   vId128_1    ));
    EXPECT_FALSE(evalToBool<fun::cmn_le>(       vId128_1,   vUint8_1    ));
    EXPECT_TRUE (evalToBool<fun::cmn_le>(       vId128_1,   vId128_1    ));
}
