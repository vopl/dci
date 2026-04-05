/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
