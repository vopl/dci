// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
