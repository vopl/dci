/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include "interface/call.hpp"

#include <vector>
#include <string>

using namespace dci;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(idl, interface_callCpMv)
{






    //non-generic, by copy
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](String a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
        };

        String s("42");
        r->as(s);
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "42");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
        };

        String s("42");
        r->as(s);
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "42");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };

        String s("42");
        r->as(s);
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "42");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](const String& a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
        };

        String s("42");
        r->as(s);
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "42");
    }











    //non-generic, by move
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](String a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "42");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slotCalled = false;
        o->as() += [&](const String& a)
        {
            EXPECT_EQ(a, "42");
            slotCalled = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slotCalled);
        EXPECT_EQ(s, "42");
    }











    //non-generic, by move, multiple
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](String a)
        {
            EXPECT_EQ(a, "42");
            slot1Called = true;
        };
        o->as() += [&](String a)
        {
            EXPECT_EQ(a, "42");
            slot2Called = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slot1Called = true;
        };
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slot2Called = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "42");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slot1Called = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };
        o->as() += [&](String&& a)
        {
            EXPECT_EQ(a, "42");
            slot2Called = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](const String& a)
        {
            EXPECT_EQ(a, "42");
            slot1Called = true;
        };
        o->as() += [&](const String& a)
        {
            EXPECT_EQ(a, "42");
            slot2Called = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "42");
    }










    //generic, by move, multiple
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](auto a)
        {
            EXPECT_TRUE((std::is_same_v<String, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot1Called = true;
        };
        o->as() += [&](auto a)
        {
            EXPECT_TRUE((std::is_same_v<String, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot2Called = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](auto&& a)
        {
            EXPECT_TRUE((std::is_same_v<const String&, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot1Called = true;
        };
        o->as() += [&](auto&& a)
        {
            EXPECT_TRUE((std::is_same_v<String&&, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot2Called = true;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "42");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](auto&& a)
        {
            EXPECT_TRUE((std::is_same_v<const String&, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot1Called = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };
        o->as() += [&](auto&& a)
        {
            EXPECT_TRUE((std::is_same_v<String&&, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot2Called = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    {
        h3::Rectangle<> r; r.init();
        h3::Object<>::Opposite o(r);

        bool slot1Called = false;
        bool slot2Called = false;
        o->as() += [&](const auto& a)
        {
            EXPECT_TRUE((std::is_same_v<const String&, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot1Called = true;
        };
        o->as() += [&](const auto& a)
        {
            EXPECT_TRUE((std::is_same_v<const String&, decltype(a)>));
            EXPECT_EQ(a, "42");
            slot2Called = true;
            String utilizer(std::move(a));
            (void)utilizer;
        };

        String s("42");
        r->as(std::move(s));
        EXPECT_TRUE(slot1Called);
        EXPECT_TRUE(slot2Called);
        EXPECT_EQ(s, "42");
    }

}
