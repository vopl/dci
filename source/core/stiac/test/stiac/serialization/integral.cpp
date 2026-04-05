/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include "utils/hereThere.hpp"
using namespace utils;

#include <dci/primitives.hpp>

using namespace dci;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, integral)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( true  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( false ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( uint8(0)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(1)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(2)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(63)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(64)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(65)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(126)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(127)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-1)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-2)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-63)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-64)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-65)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-126)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-127)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint8(-128)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( uint16(0)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(13)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(31)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(32)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(33)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(310)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(32766)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(32767)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-13)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-31)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-32)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-33)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-310)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-32766)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-32767)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint16(-32768)   ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( uint32(0)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(1)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(13)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(31)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(32)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(33)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(310)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(32766)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(32767)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(1234567)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(1234568)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(2147483647)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(2147483648)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-1)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-13)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-31)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-32)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-33)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-310)         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-32766)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-32767)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-1234567)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-1234568)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-2147483647)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint32(-2147483648)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2( uint64(0)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(1)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(13)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(31)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(32)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(33)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(310)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(32766)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(32767)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(1234567)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(1234568)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(2147483647)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(2147483648)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-1)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-13)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-31)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-32)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-33)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-310)         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-32766)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-32767)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-1234567)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-1234568)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-2147483647)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(-2147483648)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    hereThere2( uint64(0x8000000000000000ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x4000000000000000ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x2000000000000000ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    hereThere2( uint64(0x8000000000000010ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x4000000000000010ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x2000000000000010ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    hereThere2( uint64(0x800000000000aaaall)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x4000000000005555ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x2000000000005a5all)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2( uint64(0x000000000000a5a5ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int8(0)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(1)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(2)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(63)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(64)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(65)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(126)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(127)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-1)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-2)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-63)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-64)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-65)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-126)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-127)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int8(-128)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int16(0)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(13)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(31)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(32)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(33)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(310)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(32766)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(32767)    ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-13)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-31)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-32)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-33)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-310)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-32766)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-32767)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int16(-32768)   ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int32(0)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(1)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(13)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(31)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(32)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(33)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(310)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(32766)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(32767)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(1234567)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(1234568)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(2147483647)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(2147483648)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-1)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-13)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-31)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-32)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-33)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-310)         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-32766)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-32767)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-1234567)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-1234568)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-2147483647)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int32(-2147483648)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int64(0)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(1)            ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(13)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(31)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(32)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(33)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(310)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(32766)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(32767)        ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(1234567)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(1234568)      ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(2147483647)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(2147483648)   ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-1)           ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-13)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-31)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-32)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-33)          ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-310)         ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-32766)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-32767)       ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-1234567)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-1234568)     ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-2147483647)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(-2147483648)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int64(0x8000000000000000ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x4000000000000000ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x2000000000000000ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int64(0x8000000000000010ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x4000000000000010ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x2000000000000010ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    hereThere2(  int64(0x800000000000aaaall)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x4000000000005555ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x2000000000005a5all)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
    hereThere2(  int64(0x000000000000a5a5ll)  ,[](auto a, auto b){EXPECT_EQ(a, b);});
}

namespace
{
    std::size_t bufferSizeFor(auto&& v)
    {
        dci::Bytes buffer;
        moc::Hub hub;

        dci::stiac::link::Sink{&hub, buffer.begin()} << std::forward<decltype(v)>(v);

        return buffer.size();
    }

    template<class T>
    void checkOneSize(std::size_t period)
    {
        T v{};

        for(std::size_t i{0}; i<sizeof(T)*8+period; ++i)
        {
            v <<= 1;

            if(!(i%period)) v |= T{1};

            EXPECT_EQ(sizeof(T), bufferSizeFor(v));
        }
    }

    template<class T>
    void checkOneSize()
    {
        checkOneSize<T>(1);
        checkOneSize<T>(2);
        checkOneSize<T>(3);
        checkOneSize<T>(4);
        checkOneSize<T>(5);
        checkOneSize<T>(6);
        checkOneSize<T>(7);
        checkOneSize<T>(8);
        checkOneSize<T>(9);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(stiac, integral_size)
{
    checkOneSize<char>();
    checkOneSize<signed char>();
    checkOneSize<unsigned char>();

    checkOneSize<signed short>();
    checkOneSize<unsigned short>();

    checkOneSize<signed int>();
    checkOneSize<unsigned int>();

    checkOneSize<signed long>();
    checkOneSize<unsigned long>();

    checkOneSize<signed long long>();
    checkOneSize<unsigned long long>();


    checkOneSize<int8>();
    checkOneSize<int16>();
    checkOneSize<int32>();
    checkOneSize<int64>();

    checkOneSize<uint8>();
    checkOneSize<uint16>();
    checkOneSize<uint32>();
    checkOneSize<uint64>();
}
