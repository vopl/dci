/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/cmt.hpp>
#include <dci/sbs.hpp>

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, sbsVR)
{
    int progress = 0;
    dci::cmt::spawn() += [&]
    {
        dci::sbs::Wire<void, int> wire;

        dci::cmt::task::Owner tol;
        dci::sbs::Owner sol;

        std::map<int, int> res;

        wire.out() += [&](int a)
        {
            res[0] = a+0;
        };

        wire.out() += tol * [&](int a)
        {
            res[1] = a+1;
        };

        wire.out() += sol * tol * [&](int a)
        {
            res[2] = a+2;
        };

        wire.out() += tol * sol * [&](int a)
        {
            res[3] = a+3;
        };

        wire.in<dci::sbs::wire::Agg::all>(42);

        while(res.size() < 4)
            dci::cmt::yield();

        ASSERT_EQ(res.size(), 4);
        EXPECT_EQ(res[0], 42);
        EXPECT_EQ(res[1], 43);
        EXPECT_EQ(res[2], 44);
        EXPECT_EQ(res[3], 45);

        progress++;
    };

    dci::cmt::executeReadyFibers();
    EXPECT_EQ(progress, 1);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, sbsR)
{
    int progress = 0;
    dci::cmt::spawn() += [&]
    {
        dci::sbs::Wire<dci::cmt::Future<int>, int> wire;

        dci::cmt::task::Owner tol;
        dci::sbs::Owner sol;

        wire.out() += [](int a) -> dci::cmt::Future<int>
        {
            return dci::cmt::readyFuture(a+0);
        };

        wire.out() += tol * [](int a) -> int
        {
            return a+1;
        };

        wire.out() += sol * tol * [](int a) -> int
        {
            return a+2;
        };

        wire.out() += tol * sol * [](int a) -> int
        {
            return a+3;
        };

        std::vector<dci::cmt::Future<int>> res = wire.in<dci::sbs::wire::Agg::all>(42);
        ASSERT_EQ(res.size(), 4);
        EXPECT_EQ(res[0].value(), 42);
        EXPECT_EQ(res[1].value(), 43);
        EXPECT_EQ(res[2].value(), 44);
        EXPECT_EQ(res[3].value(), 45);

        progress++;
    };

    dci::cmt::executeReadyFibers();
    EXPECT_EQ(progress, 1);
}
