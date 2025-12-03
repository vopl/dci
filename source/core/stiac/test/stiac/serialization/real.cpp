// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/primitives.hpp>

using namespace dci;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, real)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( real32(0)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(1)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-1)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(0.5)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-0.5)         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-1.2345678)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-9.8765432)   ,[](auto a, auto b){EXPECT_EQ(a, b);});

    hereThere2( real32( std::numeric_limits<real32>::min())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-std::numeric_limits<real32>::min())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32( std::numeric_limits<real32>::lowest())           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-std::numeric_limits<real32>::lowest())           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32( std::numeric_limits<real32>::max())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-std::numeric_limits<real32>::max())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32( std::numeric_limits<real32>::epsilon())          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-std::numeric_limits<real32>::epsilon())          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32( std::numeric_limits<real32>::round_error())      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-std::numeric_limits<real32>::round_error())      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32( std::numeric_limits<real32>::infinity())         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32(-std::numeric_limits<real32>::infinity())         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real32( std::numeric_limits<real32>::quiet_NaN())        ,[](auto a, auto b){EXPECT_EQ(0, ::memcmp(&a, &b, sizeof(a)));});
    hereThere2( real32( std::numeric_limits<real32>::signaling_NaN())    ,[](auto a, auto b){EXPECT_EQ(0, ::memcmp(&a, &b, sizeof(a)));});
    hereThere2( real32( std::numeric_limits<real32>::denorm_min())       ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( real64(0)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(1)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-1)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(0.5)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-0.5)         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-1.2345678)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-9.8765432)   ,[](auto a, auto b){EXPECT_EQ(a, b);});

    hereThere2( real64( std::numeric_limits<real64>::min())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-std::numeric_limits<real64>::min())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64( std::numeric_limits<real64>::lowest())           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-std::numeric_limits<real64>::lowest())           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64( std::numeric_limits<real64>::max())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-std::numeric_limits<real64>::max())              ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64( std::numeric_limits<real64>::epsilon())          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-std::numeric_limits<real64>::epsilon())          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64( std::numeric_limits<real64>::round_error())      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-std::numeric_limits<real64>::round_error())      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64( std::numeric_limits<real64>::infinity())         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64(-std::numeric_limits<real64>::infinity())         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( real64( std::numeric_limits<real64>::quiet_NaN())        ,[](auto a, auto b){EXPECT_EQ(0, ::memcmp(&a, &b, sizeof(a)));});
    hereThere2( real64( std::numeric_limits<real64>::signaling_NaN())    ,[](auto a, auto b){EXPECT_EQ(0, ::memcmp(&a, &b, sizeof(a)));});
    hereThere2( real64( std::numeric_limits<real64>::denorm_min())       ,[](auto a, auto b){EXPECT_EQ(a, b);});
}
