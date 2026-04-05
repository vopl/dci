/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/sbs.hpp>
#include <dci/primitives.hpp>

using namespace dci;
using namespace dci::sbs;


/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(sbs, transfer)
{
    // void not connected
    {
        Wire<> w;
        EXPECT_NO_THROW(w.in());
    }

    // nonvoid not connected
    {
        Wire<int> w;
        EXPECT_THROW(w.in(), sbs::exception::WireNotConnected);
    }

    // copy - copy
    {
        Wire<void, String> w;
        w.out().connect([](const String& str)
        {
            EXPECT_EQ(str, "str");
        });

        String str{"str"};
        w.in(str);
        EXPECT_EQ(str, "str");
    }

    // copy - move
    {
        Wire<void, String> w;
        w.out().connect([](String&& str)
        {
            EXPECT_EQ(str, "str");
            str.clear();
        });

        String str{"str"};
        w.in(str);
        EXPECT_EQ(str, "str");
    }

    // move - copy
    {
        Wire<void, String> w;
        w.out().connect([](const String& str)
        {
            EXPECT_EQ(str, "str");
        });

        String str{"str"};
        w.in(std::move(str));
        EXPECT_EQ(str, "str");
    }

    // move - move
    {
        Wire<void, String> w;
        w.out().connect([](String&& str)
        {
            EXPECT_EQ(str, "str");
            str.clear();
        });

        String str{"str"};
        w.in(std::move(str));
        EXPECT_EQ(str, "");
    }

    // move - moveN
    {
        Wire<void, String> w;
        w.out().connect([](String&& str)
        {
            EXPECT_EQ(str, "str");
            str = "mod1";
        });
        w.out().connect([](String&& str)
        {
            EXPECT_EQ(str, "str");
            str = "mod2";
        });
        w.out().connect([](String&& str)
        {
            EXPECT_EQ(str, "str");
            str = "mod3";
        });

        String str{"str"};
        w.in(std::move(str));
        EXPECT_EQ(str, "mod3");
    }

    //agg
    {
        Wire<String> w;
        w.out().connect([]()
        {
            return String{"str1"};
        });
        w.out().connect([]()
        {
            return String{"str2"};
        });
        w.out().connect([]()
        {
            return String{"str3"};
        });

        EXPECT_EQ(w.in<wire::Agg::first>(), "str1");
        EXPECT_EQ(w.in<wire::Agg::last>(), "str3");
        EXPECT_EQ(w.in<wire::Agg::all>(), (std::vector<String>{"str1", "str2", "str3"}));
    }
}
