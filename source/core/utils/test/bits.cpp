/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/utils/bits.hpp>

#include <algorithm>

using namespace dci::utils::bits;

static const std::uint8_t testset8[256] = {
    0xe6, 0xe3, 0xd2, 0xd8, 0x3e, 0xbb, 0x2e, 0x0f, 0x8c, 0xca, 0xff, 0x7a, 0xc1, 0xc7, 0x4c, 0x61,
    0xa5, 0x58, 0xe9, 0x60, 0x94, 0xd4, 0x56, 0x06, 0x0d, 0x8e, 0xe1, 0x2f, 0x5b, 0x37, 0xf1, 0xe2,
    0xcf, 0x81, 0xa9, 0x35, 0x15, 0x6e, 0xaa, 0xf4, 0x11, 0x1c, 0x47, 0x2c, 0x0e, 0x50, 0x5e, 0xcb,
    0x68, 0xcd, 0x16, 0x28, 0xdd, 0x55, 0x6f, 0xac, 0xb5, 0xb9, 0x72, 0x80, 0x4b, 0x24, 0xb3, 0xb7,
    0x1f, 0xc4, 0x25, 0x02, 0x84, 0x13, 0x69, 0xea, 0x20, 0xf3, 0x5d, 0x9f, 0x91, 0xa0, 0x2a, 0x49,
    0x8f, 0x31, 0x9a, 0x00, 0x27, 0x76, 0x5c, 0x7e, 0x04, 0x57, 0xee, 0x08, 0x26, 0x3a, 0x75, 0x01,
    0x4f, 0x93, 0x67, 0xc2, 0x2b, 0xa3, 0xbc, 0xa1, 0xc3, 0xc5, 0x86, 0x89, 0x83, 0xf7, 0xfd, 0x59,
    0x9c, 0xf6, 0x40, 0x63, 0x52, 0x30, 0xb1, 0x53, 0x03, 0xab, 0xfb, 0xd6, 0xd9, 0x14, 0x1b, 0x29,
    0xde, 0xa2, 0x33, 0x78, 0xc0, 0xc6, 0x65, 0x6a, 0xba, 0xc8, 0xd5, 0xa7, 0x3b, 0x34, 0x3c, 0x87,
    0x2d, 0xf2, 0xa6, 0x38, 0x82, 0x9e, 0x4d, 0x7f, 0xdc, 0xf8, 0x32, 0x07, 0x43, 0xe8, 0x7c, 0x95,
    0x5f, 0xe7, 0xbe, 0xbf, 0x18, 0x3d, 0xae, 0x23, 0x79, 0xd1, 0xb6, 0x96, 0x6c, 0x1d, 0x97, 0x85,
    0x41, 0x77, 0xb0, 0xed, 0x8b, 0x36, 0xad, 0xfa, 0xdf, 0x45, 0x17, 0xe0, 0xbd, 0x54, 0x39, 0x1e,
    0xaf, 0x3f, 0xec, 0xb2, 0xb8, 0xfc, 0x0b, 0x99, 0x7d, 0xdb, 0x71, 0xd7, 0xb4, 0xc9, 0x6d, 0x1a,
    0x70, 0x10, 0x6b, 0x12, 0x88, 0x9d, 0x7b, 0x22, 0x46, 0xfe, 0xa8, 0x74, 0x73, 0x4e, 0x0a, 0x44,
    0xeb, 0xf0, 0xe4, 0x48, 0x8a, 0xce, 0xe5, 0x8d, 0xd3, 0x9b, 0x19, 0x64, 0xd0, 0x4a, 0x21, 0x09,
    0x0c, 0xef, 0x66, 0x51, 0xf5, 0x90, 0xcc, 0xf9, 0x62, 0xa4, 0x42, 0x92, 0x98, 0x05, 0xda, 0x5a,
};

template <class Integral> Integral get(std::size_t index)
{
    Integral res = static_cast<Integral>(testset8[(index+0)%256]);

    if(sizeof(res) >= 16)
    {
        res |= static_cast<Integral>(static_cast<std::uint16_t>(testset8[(index+41)%256])<<8);
    }

    if(sizeof(res) >= 32)
    {
        res |= static_cast<Integral>(static_cast<std::uint32_t>(testset8[(index+43)%256])<<16);
        res |= static_cast<Integral>(static_cast<std::uint32_t>(testset8[(index+47)%256])<<24);
    }

    if(sizeof(res) >= 64)
    {
        res |= static_cast<Integral>(static_cast<std::uint64_t>(testset8[(index+53)%256])<<32);
        res |= static_cast<Integral>(static_cast<std::uint64_t>(testset8[(index+59)%256])<<40);
        res |= static_cast<Integral>(static_cast<std::uint64_t>(testset8[(index+61)%256])<<48);
        res |= static_cast<Integral>(static_cast<std::uint64_t>(testset8[(index+67)%256])<<54);
    }

    return res;
}

template <class Integral> void test_count1Least()
{
    ASSERT_EQ(0u, count1Least(Integral(0)));
    ASSERT_EQ(bitsof<Integral>(), count1Least(Integral(~Integral(0))));

    for(std::size_t i(0); i<sizeof(Integral)*256; ++i)
    {
        Integral v = get<Integral>(i);

        for(std::size_t b(0); b<bitsof<Integral>(); ++b)
        {
            Integral mask1 = static_cast<Integral>((1ull<<b)-1);
            Integral mask2 = static_cast<Integral>(~(1ull<<b));
            Integral vv = (v | mask1) & mask2;

            //std::cout<<"0x"<<std::setw(sizeof(Integral)*2)<<std::setfill('0')<<std::hex<<(std::uint64_t)vv<<std::endl;
            ASSERT_EQ(b, count1Least(vv));
        }
    }
}

template <class Integral> void test_count1Most()
{
    ASSERT_EQ(0u, count1Most(Integral(0)));
    ASSERT_EQ(bitsof<Integral>(), count1Most(Integral(~Integral(0))));

    for(std::size_t i(0); i<sizeof(Integral)*256; ++i)
    {
        Integral v = get<Integral>(i);

        for(std::size_t b(0); b<bitsof<Integral>(); ++b)
        {
            Integral mask1 = static_cast<Integral>(~((1ull<<(bitsof<Integral>() - b - 1))-1));
            Integral mask2 = static_cast<Integral>(~(1ull<<(bitsof<Integral>() - b - 1)));
            Integral vv = (v | mask1) & mask2;

            //std::cout<<"0x"<<std::setw(sizeof(Integral)*2)<<std::setfill('0')<<std::hex<<(std::uint64_t)vv<<std::endl;
            ASSERT_EQ(b, count1Most(vv));
        }
    }
}

template <class Integral> void test_count0Least()
{
    ASSERT_EQ(bitsof<Integral>(), count0Least(Integral(0)));
    ASSERT_EQ(0u, count0Least(Integral(~Integral(0))));

    for(std::size_t i(0); i<sizeof(Integral)*256; ++i)
    {
        Integral v = get<Integral>(i);

        for(std::size_t b(0); b<bitsof<Integral>(); ++b)
        {
            Integral mask1 = static_cast<Integral>(~((1ull<<b)-1));
            Integral mask2 = static_cast<Integral>(1ull<<b);
            Integral vv = (v & mask1) | mask2;

            //std::cout<<"0x"<<std::setw(sizeof(Integral)*2)<<std::setfill('0')<<std::hex<<(std::uint64_t)vv<<std::endl;
            ASSERT_EQ(b, count0Least(vv));
        }
    }
}

template <class Integral> void test_count0Most()
{
    ASSERT_EQ(bitsof<Integral>(), count0Most(Integral(0)));
    ASSERT_EQ(0u, count0Most(Integral(~Integral(0))));

    for(std::size_t i(0); i<sizeof(Integral)*256; ++i)
    {
        Integral v = get<Integral>(i);

        for(std::size_t b(0); b<bitsof<Integral>(); ++b)
        {
            Integral mask1 = static_cast<Integral>((1ull<<(bitsof<Integral>() - b - 1))-1);
            Integral mask2 = static_cast<Integral>(1ull<<(bitsof<Integral>() - b - 1));
            Integral vv = (v & mask1) | mask2;

            //std::cout<<"0x"<<std::setw(sizeof(Integral)*2)<<std::setfill('0')<<std::hex<<(std::uint64_t)vv<<std::endl;
            ASSERT_EQ(b, count0Most(vv));
        }
    }
}

template <class Integral> void test_all()
{
    test_count1Least<std::make_signed_t<Integral>>();
    test_count1Most <std::make_signed_t<Integral>>();
    test_count0Least<std::make_signed_t<Integral>>();
    test_count0Most <std::make_signed_t<Integral>>();

    test_count1Least<std::make_unsigned_t<Integral>>();
    test_count1Most <std::make_unsigned_t<Integral>>();
    test_count0Least<std::make_unsigned_t<Integral>>();
    test_count0Most <std::make_unsigned_t<Integral>>();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, bits)
{
    static_assert( 8 == bitsof<std::uint8_t>());
    static_assert(16 == bitsof<std::uint16_t>());
    static_assert(32 == bitsof<std::uint32_t>());
    static_assert(64 == bitsof<std::uint64_t>());

    static_assert( 8 == bitsof<std::int8_t>());
    static_assert(16 == bitsof<std::int16_t>());
    static_assert(32 == bitsof<std::int32_t>());
    static_assert(64 == bitsof<std::int64_t>());

    static_assert(64 == count0Least(std::int64_t{}));
    static_assert(0  == count1Least(std::int64_t{}));
    static_assert(64 == count0Most(std::int64_t{}));
    static_assert(0  == count1Most(std::int64_t{}));

    test_all<std::uint8_t>();
    test_all<std::uint16_t>();
    test_all<std::uint32_t>();
    test_all<std::uint64_t>();
}
