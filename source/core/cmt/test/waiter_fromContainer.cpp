// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, waiter_fromContainer)
{
    bool allDone = false;
    spawn() += [&]
    {
        Event e1, e2;
        std::vector<Event*> es;
        es.emplace_back(&e2);

        auto res1 = whenAny(e1, es);
        auto res2 = whenAll(e1, es);

        yield();

        EXPECT_FALSE(res1.resolved());
        EXPECT_FALSE(res2.resolved());

        e1.raise();

        EXPECT_TRUE(res1.waitValue());
        EXPECT_EQ(res1.value(), 0u);

        EXPECT_TRUE(res1.resolved());
        EXPECT_FALSE(res2.resolved());

        e2.raise();

        EXPECT_TRUE(res2.waitValue());

        EXPECT_TRUE(res1.resolved());
        EXPECT_TRUE(res2.resolved());

        allDone = true;
    };

    executeReadyFibers();

    EXPECT_TRUE(allDone);
}
