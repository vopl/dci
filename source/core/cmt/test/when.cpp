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
