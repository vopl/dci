// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;
using namespace dci::cmt::task;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_owner1)
{
    spawn() += [&]
    {
        Owner wo;
        EXPECT_TRUE(wo.empty());
        wo.flush();//should not affect


        bool workerStart = false;
        bool workerDone = false;
        bool workerInterrupted = false;
        spawn() += [&]
        {
            workerStart = true;

            Face tf = current();

            EXPECT_TRUE(wo.empty());

            tf.ownTo(&wo);

            EXPECT_FALSE(wo.empty());

            Event e;

            try
            {
                e.wait();//hold worker
            }
            catch(const Stop&)
            {
                //interrupted
                workerInterrupted = true;
            }

            workerDone = true;
        };

        yield();

        EXPECT_FALSE(workerInterrupted);
        EXPECT_FALSE(workerDone);
        EXPECT_TRUE(workerStart);

        wo.stop(false);
        EXPECT_FALSE(workerInterrupted);
        EXPECT_FALSE(workerDone);
        EXPECT_TRUE(workerStart);
        yield();

        EXPECT_TRUE(workerInterrupted);
        EXPECT_TRUE(workerDone);
        EXPECT_TRUE(workerStart);
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_owner2)
{
    spawn() += [&]
    {
        Owner wo;
        EXPECT_TRUE(wo.empty());
        wo.flush();//should not affect

        bool workerStart = false;
        bool workerDone = false;
        bool workerInterrupted = false;
        spawn() += wo * [&]
        {
            workerStart = true;

            EXPECT_FALSE(wo.empty());

            Event e;

            try
            {
                e.wait();//hold worker
            }
            catch(const Stop&)
            {
                //interrupted
                workerInterrupted = true;
            }

            workerDone = true;
        };

        yield();

        EXPECT_FALSE(workerInterrupted);
        EXPECT_FALSE(workerDone);
        EXPECT_TRUE(workerStart);

        wo.stop(false);
        EXPECT_FALSE(workerInterrupted);
        EXPECT_FALSE(workerDone);
        EXPECT_TRUE(workerStart);
        yield();

        EXPECT_TRUE(workerInterrupted);
        EXPECT_TRUE(workerDone);
        EXPECT_TRUE(workerStart);
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_owner3)
{
    spawn() += [&]
    {
        Owner wo;
        wo.stop();

        bool workerStart = false;
        spawn() += wo * [&]
        {
            workerStart = true;
        };

        EXPECT_FALSE(workerStart);
        yield();
        EXPECT_FALSE(workerStart);
        wo.stop();
        EXPECT_FALSE(workerStart);
        yield();
        EXPECT_FALSE(workerStart);
    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_owner4)
{
    spawn() += [&]
    {
        Event evt;

        Owner owner1;
        Owner owner2;

        spawn() += owner1 * [&]
        {
            try { evt.wait(); } catch(...){}
            owner2.stop();
        };

        spawn() += owner2 * [&]
        {
            try { evt.wait(); } catch(...){}
            owner1.stop();
        };
        yield();
        evt.raise();
    };

    executeReadyFibers();
}
