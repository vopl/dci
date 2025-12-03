// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, wait1)
{
    spawn() += []
    {
        {
            Event e;

            auto res1 = spawnv() += [&]{
                return waitAny(e);
            };
            auto res2 = spawnv() += [&]{
                waitAll(e);
            };

            yield();

            EXPECT_FALSE(res1.resolved());
            EXPECT_FALSE(res2.resolved());

            e.raise();

            EXPECT_TRUE(res1.waitValue());
            EXPECT_TRUE(res2.waitValue());

            EXPECT_EQ(res1.value(), 0u);
        }
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, wait2)
{
    spawn() += []
    {
        {
            Event e;

            auto res1 = spawnv() += [&]{
                return waitAny(e);
            };
            auto res2 = spawnv() += [&]{
                waitAll(e);
            };

            yield();

            EXPECT_FALSE(res1.resolved());
            EXPECT_FALSE(res2.resolved());

            e.raise();

            EXPECT_TRUE(res1.waitValue());
            EXPECT_TRUE(res2.waitValue());

            EXPECT_EQ(res1.value(), 0u);
        }

        {
            Event e0, e1;

            auto res1 = spawnv() += [&]{
                return waitAny(e0, e1);
            };
            auto res2 = spawnv() += [&]{
                waitAll(e0, e1);
            };

            yield();

            EXPECT_FALSE(res1.resolved());
            EXPECT_FALSE(res2.resolved());

            e1.raise();

            EXPECT_TRUE(res1.waitValue());
            EXPECT_EQ(res1.value(), 1u);
            EXPECT_FALSE(res2.resolved());

            e0.raise();

            EXPECT_TRUE(res1.waitValue());
            EXPECT_TRUE(res2.waitValue());
        }
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waitExpr)
{
    bool done{};
    spawn() += [&]
    {
        {
            Event e0, e1, e2;

            auto res1 = spawnv() += [&]{
                return wait(e1 || e0);
            };
            auto res2 = spawnv() += [&]{
                return wait(e1 && e0);
            };
            auto res3 = spawnv() += [&]{
                return wait(!e2 && e1 && e0);
            };

            yield();

            e2.raise();

            EXPECT_FALSE(res1.resolved());
            EXPECT_FALSE(res2.resolved());
            EXPECT_FALSE(res3.resolved());

            e1.raise();

            EXPECT_TRUE(res1.waitValue());
            EXPECT_EQ(res1.value(), (std::bitset<2>{0b10}));
            EXPECT_FALSE(res2.resolved());
            EXPECT_FALSE(res3.resolved());

            e0.raise();

            EXPECT_TRUE(res2.waitValue());
            EXPECT_EQ(res2.value(), (std::bitset<2>{0b11}));
            EXPECT_FALSE(res3.resolved());

            e0.raise();
            e2.reset();

            EXPECT_TRUE(res3.waitValue());
            EXPECT_EQ(res3.value(), (std::bitset<3>{0b011}));

            done = true;
        }
    };

    executeReadyFibers();
    EXPECT_TRUE(done);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waitExprNegative)
{
    bool done{};
    spawn() += [&]
    {
        Event e;
        e.raise();

        auto res = spawnv() += [&]{
            return wait(!e);
        };

        yield();

        EXPECT_FALSE(res.resolved());

        e.reset();

        EXPECT_TRUE(res.waitValue());
        EXPECT_EQ(res.value(), (std::bitset<1>{0b0}));

        done = true;
    };

    executeReadyFibers();
    EXPECT_TRUE(done);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waitExprNegative2)
{
    bool done{};
    spawn() += [&]
    {
        Event e1, e2;
        e1.raise();
        e2.raise();

        auto res = spawnv() += [&]{
            return wait(!e1 && !e2);
        };

        yield();

        EXPECT_FALSE(res.resolved());

        e1.reset();

        EXPECT_FALSE(res.resolved());

        e2.reset();

        EXPECT_TRUE(res.waitValue());
        EXPECT_EQ(res.value(), (std::bitset<2>{0b00}));

        done = true;
    };

    executeReadyFibers();
    EXPECT_TRUE(done);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waitAnyWithRepeats)
{
    spawn() += []
    {
        Event e1,e2;

        auto res = spawnv() += [&]{
            return waitAny(e1,e1,e1,e2,e1,e2);
        };

        yield();

        EXPECT_FALSE(res.resolved());

        e1.raise();

        EXPECT_TRUE(res.waitValue());
        EXPECT_EQ(res.value(), 0u);
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waitAllWithRepeats)
{
    spawn() += []
    {
        Event e1,e2;

        auto res = spawnv() += [&]{
            return waitAll(e1,e1,e1,e2,e1,e2);
        };

        yield();

        EXPECT_FALSE(res.resolved());

        e1.raise();
        e2.raise();

        EXPECT_TRUE(res.waitValue());
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waitExprWithRepeats)
{
    spawn() += []
    {
        Event e1,e2;

        auto res = spawnv() += [&]{
            return wait(e1||e1||e1||e2||e1||e2);
        };

        yield();

        EXPECT_FALSE(res.resolved());

        e1.raise();

        EXPECT_TRUE(res.waitValue());
        EXPECT_EQ(res.value(), (std::bitset<6>{0b111010}));
    };

    executeReadyFibers();
}
