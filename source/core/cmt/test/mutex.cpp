// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, mutex)
{
    auto body = [](RecursionMode recursionMode)
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //construction/destruction
        {
            int progress = 0;
            int progress2 = 0;
            {
                Mutex m(recursionMode);

                EXPECT_TRUE(m.canLock());
                spawn() += [&](){progress=1;m.lock();progress=2;};
                executeReadyFibers();
                EXPECT_EQ(progress, 2);

                spawn() += [&](){progress2=1;m.lock();progress2=2;};
                executeReadyFibers();
                EXPECT_EQ(progress2, 1);
            }//m destruction
            executeReadyFibers();
            EXPECT_EQ(progress2, 2);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //secondary unlock has no effect
        spawn() += [&]()
        {
            Mutex m(recursionMode);

            EXPECT_TRUE(m.canLock());

            m.unlock();
            EXPECT_TRUE(m.canLock());

            m.lock();
            m.unlock();
            EXPECT_TRUE(m.canLock());

            m.unlock();
            EXPECT_TRUE(m.canLock());

            m.unlock();
            EXPECT_TRUE(m.canLock());
        };
        executeReadyFibers();

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //wait and lock are equal
        {
            int progress = 0;
            Mutex m(recursionMode);

            progress = 0;
            spawn() += [&](){m.wait();progress++;};
            spawn() += [&](){m.lock();progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 1);

            m.unlock();
            executeReadyFibers();
            EXPECT_EQ(progress, 2);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //canLock / tryLock
        {
            int progress = 0;
            Mutex m(recursionMode);

            progress = 0;
            spawn() += [&](){EXPECT_TRUE(m.canLock()); EXPECT_TRUE(m.tryLock());progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 1);

            spawn() += [&](){EXPECT_FALSE(m.canLock()); EXPECT_FALSE(m.tryLock());progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 2);

            m.unlock();

            spawn() += [&](){EXPECT_TRUE(m.canLock()); EXPECT_TRUE(m.tryLock());progress++;};
            executeReadyFibers();

            m.unlock();

            spawn() += [&](){EXPECT_TRUE(m.canLock()); EXPECT_TRUE(m.tryLock());progress++;};
            executeReadyFibers();
            EXPECT_EQ(progress, 4);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //lock/unlock
        {
            int progress = 0;
            Mutex m(recursionMode);

            spawn() += [&]()
            {
                progress = 10;
                m.lock();

                spawn() += [&]()
                {
                    EXPECT_EQ(progress, 10);
                    progress = 20;
                    m.lock();
                    EXPECT_EQ(progress, 12);
                    m.unlock();
                    progress = 22;
                    m.lock();
                    EXPECT_EQ(progress, 14);
                    m.unlock();
                    progress = 24;
                    m.lock();
                    EXPECT_EQ(progress, 16);
                    m.unlock();
                    progress = 26;
                };
                yield();

                EXPECT_EQ(progress, 20);
                m.unlock();
                progress = 12;
                m.lock();
                EXPECT_EQ(progress, 22);
                m.unlock();
                progress = 14;
                m.lock();
                EXPECT_EQ(progress, 24);
                m.unlock();
                progress = 16;
            };

            executeReadyFibers();
            EXPECT_EQ(progress, 26);
        }

    };

    body(RecursionMode::nonRecursive);
    body(RecursionMode::recursive);

    //recursive mode
    {
        int progress = 0;
        Mutex m(RecursionMode::recursive);

        spawn() += [&]()
        {
            m.lock();
            m.lock();
            progress++;
            spawn() += [&]()
            {
                m.lock();
                m.lock();
                progress++;
                spawn() += [&]()
                {
                    m.lock();
                    m.lock();
                    progress++;
                    m.unlock();
                    m.unlock();
                };
                m.unlock();
                m.unlock();
            };
            m.unlock();
            m.unlock();
        };

        executeReadyFibers();
        EXPECT_EQ(progress, 3);
    }

    //nonRecursive mode
    {
        int progress = 0;

        Mutex m(RecursionMode::nonRecursive);

        spawn() += [&]()
        {
            m.lock();
            progress++;
            m.lock();
            progress++;
            m.unlock();
            m.unlock();
        };

        executeReadyFibers();
        EXPECT_EQ(progress, 1);

        m.unlock();
        executeReadyFibers();
        EXPECT_EQ(progress, 2);
    }
}
