// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, semaphore)
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //construction/destruction
    {
        int progress = 0;

        {
            Semaphore s(1);

            EXPECT_TRUE(s.canLock());

            spawn() += [&](){progress++;s.lock();progress++;};
            spawn() += [&](){progress++;s.lock();progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 3);
            EXPECT_FALSE(s.canLock());
        }
        executeReadyFibers();
        EXPECT_EQ(progress, 4);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //secondary unlock has no effect
    spawn() += [&]()
    {
        Semaphore s(1);

        EXPECT_TRUE(s.canLock());

        s.unlock();
        EXPECT_TRUE(s.canLock());

        s.lock();
        s.unlock();
        EXPECT_TRUE(s.canLock());

        s.unlock();
        EXPECT_TRUE(s.canLock());

        s.unlock();
        EXPECT_TRUE(s.canLock());
    };
    executeReadyFibers();

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //wait and lock are equal
    {
        int progress = 0;
        Semaphore s(1);

        progress = 0;
        spawn() += [&](){s.wait();progress++;};
        spawn() += [&](){s.lock();progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        s.unlock();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //canLock / tryLock
    {
        int progress = 0;
        Semaphore s(1);

        progress = 0;
        spawn() += [&](){EXPECT_TRUE(s.canLock()); EXPECT_TRUE(s.tryLock());progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        spawn() += [&](){EXPECT_FALSE(s.canLock()); EXPECT_FALSE(s.tryLock());progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 2);

        s.unlock();

        spawn() += [&](){EXPECT_TRUE(s.canLock()); EXPECT_TRUE(s.tryLock());progress++;};
        executeReadyFibers();

        s.unlock();

        spawn() += [&](){EXPECT_TRUE(s.canLock()); EXPECT_TRUE(s.tryLock());progress++;};
        executeReadyFibers();
        EXPECT_EQ(progress, 4);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //lock/unlock
    {
        int progress = 0;
        Semaphore s(1);

        spawn() += [&]()
        {
            progress = 10;
            s.lock();

            spawn() += [&]()
            {
                EXPECT_EQ(progress, 10);
                progress = 20;
                s.lock();
                EXPECT_EQ(progress, 12);
                s.unlock();
                progress = 22;
                s.lock();
                EXPECT_EQ(progress, 14);
                s.unlock();
                progress = 24;
                s.lock();
                EXPECT_EQ(progress, 16);
                s.unlock();
                progress = 26;
            };
            yield();

            EXPECT_EQ(progress, 20);
            s.unlock();
            progress = 12;
            s.lock();
            EXPECT_EQ(progress, 22);
            s.unlock();
            progress = 14;
            s.lock();
            EXPECT_EQ(progress, 24);
            s.unlock();
            progress = 16;
        };

        executeReadyFibers();
        EXPECT_EQ(progress, 26);
    }

    //depth
    {
        int progress = 0;

        {
            Semaphore s(3);

            spawn() += [&](){EXPECT_TRUE(s.canLock()); EXPECT_TRUE(s.tryLock());progress++;};
            spawn() += [&](){EXPECT_TRUE(s.canLock()); EXPECT_TRUE(s.tryLock());progress++;};
            spawn() += [&](){EXPECT_TRUE(s.canLock()); EXPECT_TRUE(s.tryLock());progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 3);

            spawn() += [&](){EXPECT_FALSE(s.canLock()); EXPECT_FALSE(s.tryLock()); s.lock(); progress++;};
            spawn() += [&](){EXPECT_FALSE(s.canLock()); EXPECT_FALSE(s.tryLock()); s.lock(); progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 3);

            s.unlock();
            executeReadyFibers();
            EXPECT_EQ(progress, 4);

            s.unlock();
            executeReadyFibers();
            EXPECT_EQ(progress, 5);
        }
    }
}
