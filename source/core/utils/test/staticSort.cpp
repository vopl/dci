/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/utils/staticSort.hpp>
#include <cstdlib>

using namespace dci::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace
{
    template <std::size_t size>
    void testOne()
    {
        for(std::size_t i{}; i<1000; ++i)
        {
            std::array<int, size> arr;
            std::generate(arr.begin(), arr.end(), rand);
            auto arr2 = arr;
            std::sort(arr.begin(), arr.end());
            staticSort(arr2);
            EXPECT_EQ(arr, arr2);
        }
    };
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, staticSort)
{
    testOne<0>();
    testOne<1>();
    testOne<2>();
    testOne<3>();
    testOne<4>();
    testOne<5>();
    testOne<6>();
    testOne<7>();
    testOne<8>();
    testOne<9>();
    testOne<10>();
    testOne<11>();
    testOne<12>();
    testOne<13>();
    testOne<14>();
    testOne<15>();
    testOne<16>();
    testOne<17>();
    testOne<18>();
    testOne<19>();
    testOne<20>();
    testOne<31>();
    testOne<42>();
    testOne<53>();
    testOne<64>();
    testOne<75>();
    testOne<86>();
    testOne<97>();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(utils, staticSort_customComparator)
{
    std::array<int, 5> arr;
    std::generate(arr.begin(), arr.end(), rand);
    auto arr2 = arr;
    std::sort(arr.begin(), arr.end(), std::greater<int>{});
    staticSort(arr2, std::greater<int>{});
    EXPECT_EQ(arr, arr2);
}
