// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, when)
{
    bool allDone = false;
    spawn() += [&]
    {
        {
            Event e;

            int progress = 0;
            whenAny(e).then() += [&](){
                progress++;
            };
            whenAll(e).then() += [&](){
                progress++;
            };
            e.raise();
            yield();
            EXPECT_EQ(progress, 2);
        }

        {
            Event e0, e1;

            int progress = 0;
            whenAny(e0, e1).then() += [&](auto f){
                progress++;
                EXPECT_EQ(f.value(), 0u);
            };
            whenAll(e0, e1).then() += [&](){
                progress++;
            };
            e0.raise();
            yield();
            EXPECT_EQ(progress, 1);

            e1.raise();
            yield();
            EXPECT_EQ(progress, 2);
        }

        {
            Event e0, e1;

            int progress = 0;
            whenAny(e0, e1).then() += [&](auto& f){
                progress++;
                EXPECT_EQ(f.value(), 1u);
            };
            whenAll(e0, e1).then() += [&](){
                progress++;
            };
            e1.raise();
            yield();
            EXPECT_EQ(progress, 1);

            e0.raise();
            yield();
            EXPECT_EQ(progress, 2);
        }

        {
            Event e;
            Pulser p;

            int progress = 0;
            whenAll(e, p).then() += [&](){
                progress++;
            };

            yield();
            EXPECT_EQ(progress, 0);

            p.raise();
            yield();
            EXPECT_EQ(progress, 0);


            e.raise();
            yield();
            EXPECT_EQ(progress, 0);
        }

        {
            Event e;
            Pulser p;

            int progress = 0;
            whenAll(e, p).then() += [&](){
                progress++;
            };

            yield();
            EXPECT_EQ(progress, 0);

            p.raise();
            yield();
            EXPECT_EQ(progress, 0);

            e.raise();
            yield();
            EXPECT_EQ(progress, 0);

            p.raise();
            yield();
            EXPECT_EQ(progress, 1);
        }

        {
            Event e;
            Mutex m(RecursionMode::nonRecursive);
            Mutex m2(RecursionMode::recursive);

            m.lock();
            m2.lock();

            int progress = 0;

            spawn() += [&]
            {
                whenAll(e, m, m2).then() += [&](){
                    progress++;
                    EXPECT_FALSE(m.canLock());
                    EXPECT_TRUE(m2.canLock());//1. recursive, 2. was locked in current coro
                };
            };

            yield();
            EXPECT_EQ(progress, 0);

            e.raise();
            yield();
            EXPECT_EQ(progress, 0);

            m.unlock();
            m2.unlock();
            yield();
            EXPECT_EQ(progress, 1);

        }

        allDone = true;
    };

    executeReadyFibers();
    EXPECT_TRUE(allDone);
}
