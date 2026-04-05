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
TEST(poll, waitableTimer)
{
    //single
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        WaitableTimer t1{std::chrono::milliseconds{1}};
        WaitableTimer t2{std::chrono::milliseconds{1}};

        t1.start();
        t2.start();

        t1.wait();
        t2.wait();
    };

    //mass
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        WaitableTimer t1{std::chrono::milliseconds{1}};
        WaitableTimer t2{std::chrono::milliseconds{1}};

        t1.start();
        t2.start();

        dci::cmt::waitAll(t1, t2);
    };

    utils::complexRun();

    EXPECT_EQ(scopeRaiiCounter, 0);
}
