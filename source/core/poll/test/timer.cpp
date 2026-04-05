/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/poll.hpp>
#include <dci/cmt.hpp>
#include "utils/complexRun.hpp"

using namespace dci::poll;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace
{
    int scopeRaiiCounter = 0;
    struct ScopeRaii
    {
        ScopeRaii(){scopeRaiiCounter++;}
        ~ScopeRaii(){scopeRaiiCounter--;}
    };
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(poll, timer)
{
    //ctor/dtor
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        {
            Timer t{std::chrono::hours{2}, true};

            EXPECT_EQ(t.interval(), std::chrono::hours{2});
            EXPECT_EQ(t.repeatable(), true);
            EXPECT_FALSE(t.started());
        }

        {
            Timer t{std::chrono::hours{2}, true};

            EXPECT_EQ(t.interval(), std::chrono::hours{2});
            EXPECT_EQ(t.repeatable(), true);
            EXPECT_FALSE(t.started());

            t.interval(std::chrono::seconds{42});
            EXPECT_EQ(t.interval(), std::chrono::seconds{42});

            t.repeatable(false);
            EXPECT_EQ(t.repeatable(), false);
        }
    };

    //start/stop
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        Timer t{std::chrono::milliseconds{1}};

        EXPECT_FALSE(t.started());

        t.start();
        EXPECT_TRUE(t.started());

        t.stop();
        EXPECT_FALSE(t.started());
    };

    //fire now
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        struct State
        {
            int cnt=0;
            dci::cmt::Event e;
        } state;

        Timer t{std::chrono::hours{0}};
        t.tick() += [&]()
        {
            state.cnt++;
            state.e.raise();
        };

        t.start();
        EXPECT_EQ(state.cnt, 0);
        state.e.wait();
        EXPECT_EQ(state.cnt, 1);

        EXPECT_FALSE(t.started());
    };

    //fire after 1
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        struct State
        {
            int cnt=0;
            dci::cmt::Event e1;
            dci::cmt::Event e2;
        } state;

        Timer t1{std::chrono::milliseconds{1}};
        t1.tick() += [&]()
        {
            state.cnt++;
            state.e1.raise();
        };

        Timer t2{std::chrono::milliseconds{2}};
        t2.tick() += [&]()
        {
            state.cnt++;
            state.e2.raise();
        };

        t1.start();
        t2.start();
        EXPECT_EQ(state.cnt, 0);
        state.e1.wait();
        state.e2.wait();

        EXPECT_EQ(state.cnt, 2);

        EXPECT_FALSE(t1.started());
        EXPECT_FALSE(t2.started());
    };

    //change during tick
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        struct State
        {
            int cnt=0;

            Timer t1;

            dci::cmt::Event e;
        } state;

        state.t1.interval(std::chrono::milliseconds{1});

        auto cb = [&]()
        {
            EXPECT_FALSE(state.t1.started());

            state.t1.start();

            EXPECT_TRUE(state.t1.started());

            state.t1.stop();

            EXPECT_FALSE(state.t1.started());

            state.cnt++;
            state.e.raise();
        };

        state.t1.tick() += cb;

        state.t1.start();

        EXPECT_EQ(state.cnt, 0);
        state.e.wait();
        EXPECT_EQ(state.cnt, 1);

        EXPECT_FALSE(state.t1.started());
    };

    //repeating
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        struct State
        {
            int cnt=0;

            Timer t1;
            Timer t2;
            dci::cmt::Event e;
        } state;

        state.t1.interval(std::chrono::milliseconds{1});
        state.t1.repeatable(true);

        state.t2.interval(std::chrono::milliseconds{1});
        state.t2.repeatable(true);

        auto cb = [&]()
        {
            EXPECT_TRUE(state.t1.started());
            EXPECT_TRUE(state.t2.started());

            state.cnt++;

            if(state.cnt > 5)
            {
                state.t1.stop();
                state.t2.stop();
                state.e.raise();

                EXPECT_FALSE(state.t1.started());
                EXPECT_FALSE(state.t2.started());
            }
        };

        state.t1.tick() += cb;
        state.t2.tick() += cb;

        state.t1.start();
        state.t2.start();

        EXPECT_EQ(state.cnt, 0);
        state.e.wait();
        EXPECT_EQ(state.cnt, 6);

        EXPECT_FALSE(state.t1.started());
        EXPECT_FALSE(state.t2.started());
    };


    //make repeating
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        struct State
        {
            int cnt=0;

            Timer t1;
            Timer t2;
            dci::cmt::Event e;
        } state;

        state.t1.interval(std::chrono::milliseconds{1});
        state.t1.repeatable(false);

        state.t2.interval(std::chrono::milliseconds{1});
        state.t2.repeatable(false);

        auto cb = [&]()
        {
            state.t1.repeatable(true);
            state.t2.repeatable(true);

            if(!state.t1.started())
            {
                state.t1.start();
            }

            if(!state.t2.started())
            {
                state.t2.start();
            }

            EXPECT_TRUE(state.t1.started());
            EXPECT_TRUE(state.t2.started());

            state.cnt++;

            if(state.cnt > 5)
            {
                state.t1.stop();
                state.t2.stop();
                state.e.raise();
            }
        };

        state.t1.tick() += cb;
        state.t2.tick() += cb;

        state.t1.start();
        state.t2.start();

        EXPECT_EQ(state.cnt, 0);
        state.e.wait();
        EXPECT_EQ(state.cnt, 6);

        EXPECT_FALSE(state.t1.started());
        EXPECT_FALSE(state.t2.started());
    };

    utils::complexRun();

    EXPECT_EQ(scopeRaiiCounter, 0);
}
