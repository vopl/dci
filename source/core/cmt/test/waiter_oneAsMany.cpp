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

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waiter_oneAsMany)
{
    bool allDone1 = false;
    bool allDone2 = false;

    spawn() += [&]
    {
        Pulser p1, p2;

        int progress = 0;
        whenAll(p1, p2).then() += [&]
        {
            progress++;
        };

        EXPECT_EQ(progress, 0);

        yield();
        EXPECT_EQ(progress, 0);

        p1.raise();
        yield();
        EXPECT_EQ(progress, 0);

        p2.raise();
        yield();
        EXPECT_EQ(progress, 0);

        p1.raise();
        yield();
        EXPECT_EQ(progress, 0);

        p1.raise();
        p2.raise();
        yield();
        EXPECT_EQ(progress, 0);

        allDone1 = true;
    };

    spawn() += [&]
    {
        Notifier n1, n2;

        int progress = 0;
        whenAll(n1, n2).then() += [&]
        {
            progress++;
        };

        EXPECT_FALSE(n1.isRaised());
        EXPECT_FALSE(n2.isRaised());
        EXPECT_EQ(progress, 0);

        yield();
        EXPECT_FALSE(n1.isRaised());
        EXPECT_FALSE(n2.isRaised());
        EXPECT_EQ(progress, 0);

        n1.raise();
        yield();
        EXPECT_TRUE(n1.isRaised());
        EXPECT_FALSE(n2.isRaised());
        EXPECT_EQ(progress, 0);

        n2.raise();
        yield();
        EXPECT_FALSE(n1.isRaised());
        EXPECT_FALSE(n2.isRaised());
        EXPECT_EQ(progress, 1);

        allDone2 = true;
    };

    executeReadyFibers();
    EXPECT_TRUE(allDone1 && allDone2);
}
