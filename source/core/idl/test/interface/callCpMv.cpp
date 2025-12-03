// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
