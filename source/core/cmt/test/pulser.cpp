// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, pulser)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //construction/destruction
    {
        int progress = 0;
        {
            Pulser p;

            spawn() += [&](){progress=1;p.wait();progress=2;};
            executeReadyFibers();
            EXPECT_EQ(progress, 1);
        }//p destruction
        executeReadyFibers();
        EXPECT_EQ(progress, 2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //non raised not wake
    {
        int progress = 0;
        Pulser p;

        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 4);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //waked reset
    {
        int progress = 0;
        Pulser p;
        p.raise();

        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();

        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 2);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 4);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raised wake - for all
    {
        int progress = 0;
        Pulser p;

        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        progress=0;
        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raised wake - for one
    {
        int progress = 0;
        Pulser p(WakeMode::one);

        p.raise();
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        progress=0;
        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //wake all
    {
        int progress = 0;
        Pulser p;

        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        spawn() += [&](){p.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //multiple
    {
        int progress = 0;
        Pulser p1, p2;

        spawn() += [&](){progress=0; p1.wait(); progress=1; p2.wait(); progress=2;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p1.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        p2.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);

        ////
        progress=0;
        spawn() += [&](){progress=0; p1.wait(); progress=1; p2.wait(); progress=2;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        p1.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        p2.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //freestyle
    {
        int progress = 0;
        {
            Pulser p;

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=1;p.wait();progress=2;};

            executeReadyFibers();
            ASSERT_EQ(progress, 1);

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            p.raise();

            executeReadyFibers();
            ASSERT_EQ(progress, 2);

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=3;p.wait();progress=4;};

            executeReadyFibers();
            ASSERT_EQ(progress, 3);

            p.raise();
            executeReadyFibers();
            ASSERT_EQ(progress, 4);

            p.raise();

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=5;p.wait();progress=6;};
            p.raise();

            executeReadyFibers();
            ASSERT_EQ(progress, 5);

            p.raise();

            executeReadyFibers();
            ASSERT_EQ(progress, 6);

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            p.raise();

            spawn() += [&](){progress=7;p.wait();progress=8;};

            executeReadyFibers();
            ASSERT_EQ(progress, 7);

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            p.raise();

            executeReadyFibers();
            ASSERT_EQ(progress, 8);

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=9;p.wait();progress=10;};
            executeReadyFibers();
            ASSERT_EQ(progress, 9);
        }
        ASSERT_EQ(progress, 9);
        executeReadyFibers();
        ASSERT_EQ(progress, 10);
    }
}
