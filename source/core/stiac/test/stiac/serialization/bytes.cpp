// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/bytes.hpp>

using namespace dci;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, bytes)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( Bytes{}                  ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    hereThere2( Bytes{"asdf"}            ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    hereThere2( Bytes{"asdf----fdsa"}    ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});

    for(std::size_t i{}; i<20; ++i)
    {
        Bytes b;
        b.end().write("012345");
        b.end().advance(static_cast<int32>(i*997));
        b.end().write("6789ab");

        hereThere2( std::move(b)    ,[](const auto& a, const auto& b){EXPECT_EQ(a, b);});
    }
}
