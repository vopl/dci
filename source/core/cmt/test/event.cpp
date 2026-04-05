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
TEST(cmt, event)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //construction/destruction
    {
        int progress = 0;
        {
            Event e;
            EXPECT_FALSE(e.isRaised());

            spawn() += [&](){progress=1;e.wait();progress=2;};
            executeReadyFibers();
            EXPECT_EQ(progress, 1);
            EXPECT_FALSE(e.isRaised());
        }//e destruction
        executeReadyFibers();
        EXPECT_EQ(progress, 2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raise/isRaised
    {
        Event e;

        e.reset();
        EXPECT_FALSE(e.isRaised());

        e.raise();
        EXPECT_TRUE(e.isRaised());

        e.reset();
        EXPECT_FALSE(e.isRaised());

        e.raise();
        EXPECT_TRUE(e.isRaised());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //non raised not wake
    {
        int progress = 0;
        Event e;

        spawn() += [&](){e.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        spawn() += [&](){e.wait();progress++;};
        spawn() += [&](){e.wait();progress++;};
        spawn() += [&](){e.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        e.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 4);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raised wake
    {
        int progress = 0;
        Event e;
        e.raise();

        spawn() += [&](){e.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        progress=0;
        spawn() += [&](){e.wait();progress++;};
        spawn() += [&](){e.wait();progress++;};
        spawn() += [&](){e.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //wake all
    {
        int progress = 0;
        Event e;

        spawn() += [&](){e.wait();e.reset();progress++;};
        spawn() += [&](){e.wait();e.reset();progress++;};
        spawn() += [&](){e.wait();e.reset();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        e.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //multiple
    {
        int progress = 0;
        Event e1, e2;

        spawn() += [&](){progress=0; e1.wait(); progress=1; e2.wait(); progress=2;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        e1.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        e2.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);

        ////
        e1.reset();
        e2.reset();

        progress=0;
        spawn() += [&](){progress=0; e1.wait();e2.wait(); progress=1;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        e1.raise();
        e2.raise();

        executeReadyFibers();
        EXPECT_EQ(progress, 1);
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //freestyle
    {
        int progress = 0;
        {
            Event e;

            EXPECT_FALSE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=1;e.wait();progress=2;};

            executeReadyFibers();
            EXPECT_EQ(progress, 1);
            EXPECT_FALSE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            e.raise();
            EXPECT_TRUE(e.isRaised());

            executeReadyFibers();
            EXPECT_EQ(progress, 2);
            EXPECT_TRUE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=3;e.wait();progress=4;};

            executeReadyFibers();
            EXPECT_EQ(progress, 4);
            EXPECT_TRUE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            e.raise();
            EXPECT_TRUE(e.isRaised());

            spawn() += [&](){progress=5;e.wait();progress=6;};

            executeReadyFibers();
            EXPECT_EQ(progress, 6);
            EXPECT_TRUE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            e.reset();
            EXPECT_FALSE(e.isRaised());

            spawn() += [&](){progress=7;e.wait();progress=8;};

            executeReadyFibers();
            EXPECT_EQ(progress, 7);
            EXPECT_FALSE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            e.raise();
            EXPECT_TRUE(e.isRaised());

            executeReadyFibers();
            EXPECT_EQ(progress, 8);
            EXPECT_TRUE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            e.reset();
            EXPECT_FALSE(e.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=9;e.wait();progress=10;};
            executeReadyFibers();
            EXPECT_EQ(progress, 9);
        }
        EXPECT_EQ(progress, 9);
        executeReadyFibers();
        EXPECT_EQ(progress, 10);
    }
}
