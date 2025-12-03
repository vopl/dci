// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;
using namespace dci::cmt::task;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_face)
{
    spawn() += [&]
    {
        bool workerStart = false;
        bool workerDone = false;
        bool workerInterrupted = false;

        Face f1;

        Notifier firstStarted;
        Notifier secondComplete;

        spawn() += [&]
        {
            firstStarted.raise();
            workerStart = true;

            f1 = current();

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

        spawn() += [&]
        {
            firstStarted.wait();
            f1.stop(false);
            secondComplete.raise();
        };

        secondComplete.wait();

        EXPECT_TRUE(workerStart);
        EXPECT_TRUE(workerInterrupted);
        EXPECT_TRUE(workerDone);

    };

    executeReadyFibers();
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_face_p)
{
    bool allDone = false;
    spawn() += [&]
    {
        Notifier started;
        Notifier interrupted;
        Notifier done;

        Promise<> prm;

        spawn() += [&]
        {
            started.raise();

            current().stopOnResolvedCancel(prm);

            Event e;
            try
            {
                e.wait();//hold worker
            }
            catch(const Stop&)
            {
                //interrupted
                interrupted.raise();
            }

            done.raise();
        };

        started.wait();

        prm.resolveCancel();

        interrupted.wait();
        done.wait();
        allDone = true;
    };

    executeReadyFibers();
    EXPECT_TRUE(allDone);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, task_face_f)
{
    bool allDone = false;

    spawn() += [&]
    {
        Notifier started;
        Notifier interrupted;
        Notifier done;

        Promise<> prm;
        Future<> fut = prm.future();

        spawn() += [&]
        {
            started.raise();

            current().stopOnResolvedValue(fut);

            Event e;
            try
            {
                e.wait();//hold worker
            }
            catch(const Stop&)
            {
                //interrupted
                interrupted.raise();
            }

            done.raise();
        };

        started.wait();

        prm.resolveValue();

        interrupted.wait();
        done.wait();
        allDone = true;
    };

    executeReadyFibers();
    EXPECT_TRUE(allDone);
}
