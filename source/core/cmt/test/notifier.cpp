// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

TEST(cmt, notifier)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //construction/destruction
    {
        int progress = 0;
        {
            Notifier n;
            EXPECT_FALSE(n.isRaised());

            spawn() += [&](){progress=1;n.wait();progress=2;};
            executeReadyFibers();
            EXPECT_EQ(progress, 1);
            EXPECT_FALSE(n.isRaised());
        }//n destruction
        executeReadyFibers();
        EXPECT_EQ(progress, 2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raise/isRaised
    {
        Notifier n;

        n.reset();
        EXPECT_FALSE(n.isRaised());

        n.raise();
        EXPECT_TRUE(n.isRaised());

        n.reset();
        EXPECT_FALSE(n.isRaised());

        n.raise();
        EXPECT_TRUE(n.isRaised());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //non raised not wake
    {
        int progress = 0;
        Notifier n;

        progress=0;
        spawn() += [&](){n.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        spawn() += [&](){n.wait();progress++;};
        spawn() += [&](){n.wait();progress++;};
        spawn() += [&](){n.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        n.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 4);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //waked reset
    {
        Notifier n;

        n.raise();
        EXPECT_TRUE(n.isRaised());

        spawn() += [&](){n.wait();};
        executeReadyFibers();

        EXPECT_FALSE(n.isRaised());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raised wake - for all
    {
        int progress = 0;
        Notifier n;

        n.raise();

        spawn() += [&](){n.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        progress=0;
        spawn() += [&](){n.wait();progress++;};
        spawn() += [&](){n.wait();progress++;};
        spawn() += [&](){n.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        n.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //raised wake - for one
    {
        int progress = 0;
        Notifier n(WakeMode::one);

        n.raise();
        spawn() += [&](){n.wait();progress++;};
        executeReadyFibers();
        EXPECT_FALSE(n.isRaised());
        EXPECT_EQ(progress, 1);

        progress=0;
        spawn() += [&](){n.wait();progress++;};
        spawn() += [&](){n.wait();progress++;};
        spawn() += [&](){n.wait();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        n.raise();
        executeReadyFibers();
        EXPECT_FALSE(n.isRaised());
        EXPECT_EQ(progress, 1);

        n.raise();
        executeReadyFibers();
        EXPECT_FALSE(n.isRaised());
        EXPECT_EQ(progress, 2);

        n.raise();
        executeReadyFibers();
        EXPECT_FALSE(n.isRaised());
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //wake all
    {
        int progress = 0;
        Notifier n;

        spawn() += [&](){n.wait();n.reset();progress++;};
        spawn() += [&](){n.wait();n.reset();progress++;};
        spawn() += [&](){n.wait();n.reset();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        n.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //multiple
    {
        int progress = 0;
        Notifier n1, n2;

        spawn() += [&](){progress=0; n1.wait(); progress=1; n2.wait(); progress=2;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        n1.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        n2.raise();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);

        ////
        n1.reset();
        n2.reset();

        progress=0;
        spawn() += [&](){progress=0; n1.wait();n2.wait(); progress=1;};
        executeReadyFibers();
        EXPECT_EQ(progress, 0);

        n1.raise();
        n2.raise();

        executeReadyFibers();
        EXPECT_EQ(progress, 1);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //freestyle
    {
        int progress = 0;
        {
            Notifier n;

            ASSERT_FALSE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=1;n.wait();progress=2;};

            executeReadyFibers();
            ASSERT_EQ(progress, 1);
            ASSERT_FALSE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            n.raise();
            ASSERT_FALSE(n.isRaised());

            executeReadyFibers();
            ASSERT_EQ(progress, 2);
            ASSERT_FALSE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=3;n.wait();progress=4;};

            executeReadyFibers();
            ASSERT_EQ(progress, 3);
            ASSERT_FALSE(n.isRaised());

            n.raise();
            ASSERT_FALSE(n.isRaised());
            executeReadyFibers();
            ASSERT_EQ(progress, 4);

            n.raise();
            ASSERT_TRUE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            n.raise();
            ASSERT_TRUE(n.isRaised());

            spawn() += [&](){progress=5;n.wait();progress=6;};

            executeReadyFibers();
            ASSERT_EQ(progress, 6);
            ASSERT_FALSE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            n.raise();
            ASSERT_TRUE(n.isRaised());
            n.reset();
            ASSERT_FALSE(n.isRaised());

            spawn() += [&](){progress=7;n.wait();progress=8;};

            executeReadyFibers();
            ASSERT_EQ(progress, 7);
            ASSERT_FALSE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            n.raise();
            ASSERT_FALSE(n.isRaised());

            executeReadyFibers();
            ASSERT_EQ(progress, 8);
            ASSERT_FALSE(n.isRaised());

            /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
            spawn() += [&](){progress=9;n.wait();progress=10;};
            executeReadyFibers();
            ASSERT_EQ(progress, 9);
        }
        ASSERT_EQ(progress, 9);
        executeReadyFibers();
        ASSERT_EQ(progress, 10);
    }
}
