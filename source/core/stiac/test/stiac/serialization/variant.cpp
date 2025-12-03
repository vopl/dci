// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/primitives/variant.hpp>
using namespace dci;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, variant)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( Variant<char>('c')       ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    hereThere2( Variant<char, int>('c')  ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    hereThere2( Variant<char, int>(220)  ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
}
