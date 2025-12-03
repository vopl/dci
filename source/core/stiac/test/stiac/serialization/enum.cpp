// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/primitives.hpp>

using namespace dci;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, enum)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // enum class
    {
        enum E1 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : uint8 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : uint16 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : uint32 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : uint64 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : uint8 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : int16 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : int32 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum E1 : int64 { x,y,z };

        hereThere2( x            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( y            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( z            ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    // enum class
    {
        enum class E1 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : uint8 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : uint16 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : uint32 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : uint64 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : uint8 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : int16 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : int32 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }

    {
        enum class E1 : int64 { x,y,z };

        hereThere2( E1::x        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::y        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1::z        ,[](auto a, auto b){EXPECT_EQ(a, b);});
        hereThere2( E1(17)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    }


}
