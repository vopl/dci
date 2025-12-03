// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
