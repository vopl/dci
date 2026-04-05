/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <compare>
#include <dci/test.hpp>
#include "mkBig.hpp"

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(bytes, cursor)
{
    //пустой
    {
        Cursor c;

        EXPECT_TRUE(c.atBegin());
        EXPECT_TRUE(c.atEnd());
        EXPECT_EQ(0u, c.pos());
        EXPECT_EQ(0u, c.size());
        EXPECT_EQ(0u, c.sizeBack());

        EXPECT_EQ(0, c.advance(0));
        EXPECT_EQ(0, c.advance(10));
        EXPECT_EQ(0, c.advance(-1));
        EXPECT_EQ(0, c.advance(-10));

        EXPECT_TRUE(c.atBegin());
        EXPECT_TRUE(c.atEnd());
        EXPECT_EQ(0u, c.pos());
        EXPECT_EQ(0u, c.size());
        EXPECT_EQ(0u, c.sizeBack());

        EXPECT_EQ(0, c.advanceChunks(1));
        EXPECT_EQ(0, c.advanceChunks(10));

        EXPECT_TRUE(c.atBegin());
        EXPECT_TRUE(c.atEnd());
        EXPECT_EQ(0u, c.pos());
        EXPECT_EQ(0u, c.size());
        EXPECT_EQ(0u, c.sizeBack());

        EXPECT_EQ(nullptr, c.continuousData());
        EXPECT_EQ(0u, c.continuousDataSize());

        EXPECT_EQ(0u, c.read(nullptr, 220));

        Bytes b;
        EXPECT_EQ(0u, c.read(b, 220));

        Alter a(b.begin());
        EXPECT_EQ(0u, c.read(a, 220));

        EXPECT_EQ(std::strong_ordering::equal, c.compare(nullptr, 0));
        EXPECT_EQ(std::strong_ordering::equal, c.compare(b));
        EXPECT_EQ(std::strong_ordering::equal, c.compare(a));

        EXPECT_EQ(String(), c.toString());
        EXPECT_EQ(String(), c.toHex());
    }

    //не пустой
    {
        Bytes b = mkBig();
        Cursor c(b.begin());

        EXPECT_TRUE(c.atBegin());
        EXPECT_FALSE(c.atEnd());
        EXPECT_EQ(0u, c.pos());
        EXPECT_EQ(b.size(), c.size());
        EXPECT_EQ(0u, c.sizeBack());

        EXPECT_EQ(0, c.advance(0));
        EXPECT_EQ(10, c.advance(10));
        EXPECT_EQ(-1, c.advance(-1));
        EXPECT_EQ(-9, c.advance(-10));

        EXPECT_TRUE(c.atBegin());
        EXPECT_TRUE(!c.atEnd());
        EXPECT_EQ(0u, c.pos());
        EXPECT_EQ(b.size(), c.size());
        EXPECT_EQ(0u, c.sizeBack());

        EXPECT_EQ(1, c.advanceChunks(1));
        EXPECT_EQ(10, c.advanceChunks(10));

        EXPECT_FALSE(c.atBegin());
        EXPECT_FALSE(c.atEnd());
        EXPECT_LT(0u, c.pos());
        EXPECT_LT(0u, c.size());
        EXPECT_LT(0u, c.sizeBack());

        EXPECT_NE(nullptr, c.continuousData());
        EXPECT_LT(0u, c.continuousDataSize());

        char ac[5];
        EXPECT_EQ(5u, c.read(ac, 5));

        Bytes b2;
        EXPECT_EQ(5u, c.read(b2, 5));
        EXPECT_EQ(5u, b2.size());

        Alter a(b2.begin());
        EXPECT_EQ(5u, c.read(a, 5));
        EXPECT_EQ(5u, b2.size());

        EXPECT_EQ(std::strong_ordering::greater, c.compare(nullptr, 0));

        EXPECT_NE(String(), c.toString());
        EXPECT_EQ(String(), c.toHex());
    }

    //промотка с разным шагом
    for(int32 step(1); step<20; ++step)
    {
        //промотка вперед
        {
            Bytes b = mkBig();
            Cursor c(b.begin());

            uint32 pos = 0;
            while(!c.atEnd())
            {
                EXPECT_EQ(pos, c.sizeBack());
                EXPECT_EQ(b.size()-pos, c.size());

                EXPECT_LE(1u, c.continuousDataSize());
                EXPECT_NE(nullptr, c.continuousData());
                EXPECT_EQ(byte(pos), *static_cast<const byte *>(c.continuousData()));

                {
                    Cursor c2(c);
                    byte ab[1];

                    EXPECT_EQ(1u, c2.read(ab, 1));
                    EXPECT_EQ(byte(pos), ab[0]);
                }

                {
                    Cursor c2(c);
                    byte ab[1] = {byte(pos)};
                    if(c.size()>1)
                        EXPECT_EQ(std::strong_ordering::greater, c2.compare(ab, 1));
                    else
                        EXPECT_EQ(std::strong_ordering::equal, c2.compare(ab, 1));
                }

                pos += static_cast<uint32>(c.advance(step));
            }
        }

        //промотка назад
        {
            Bytes b = mkBig();
            Cursor c(b.end());

            uint32 pos = b.size();
            do
            {
                pos += static_cast<uint32>(c.advance(-step));

                EXPECT_EQ(pos, c.sizeBack());
                EXPECT_EQ(b.size()-pos, c.size());

                EXPECT_LE(1u, c.continuousDataSize());
                EXPECT_NE(nullptr, c.continuousData());
                EXPECT_EQ(byte(pos), *static_cast<const byte *>(c.continuousData()));

                {
                    Cursor c2(c);
                    byte ab[1];

                    EXPECT_EQ(1u, c2.read(ab, 1));
                    EXPECT_EQ(byte(pos), ab[0]);
                }

                {
                    Cursor c2(c);
                    byte ab[1] = {byte(pos)};
                    if(c.size()>1)
                        EXPECT_EQ(std::strong_ordering::greater, c2.compare(ab, 1));
                    else
                        EXPECT_EQ(std::strong_ordering::equal, c2.compare(ab, 1));
                }
            }
            while(!c.atBegin());
        }
    }
}
