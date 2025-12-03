// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/sbs.hpp>

using namespace dci::sbs;

namespace
{
    struct Subs
        : Subscription
    {
        std::function<void(void *, std::uint_fast8_t)> _f;

        Subs(const auto& f)
            : Subscription(&Subs::activator)
            , _f(f)
        {
        }

        static void activator(Subscription*s, void* ctx, std::uint_fast8_t flags)
        {
            Subs* self = static_cast<Subs*>(s);

            if(Subscription::act & flags)
            {
                self->_f(ctx, flags);
            }

            if(Subscription::del & flags)
            {
                delete self;
            }
        }
    };
}


/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(sbs, activate)
{
    //single
    {
        Box box;

        int cnt=0;
        box.push(new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt++;
        }});
        box.activate();
        EXPECT_EQ(1, cnt);
        box.activate();
        EXPECT_EQ(2, cnt);
    }

    //multiple
    {
        Box box;

        int cnt1=0;
        int cnt2=0;
        int cnt3=0;
        box.push(new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt1++;
        }});
        box.push(new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt2++;
        }});
        box.push(new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt3++;
        }});
        box.activate();
        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(1, cnt2);
        EXPECT_EQ(1, cnt3);
        box.activate();
        EXPECT_EQ(2, cnt1);
        EXPECT_EQ(2, cnt2);
        EXPECT_EQ(2, cnt3);
    }

    //next/last, single
    {
        Box box;

        int cnt=0;
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_TRUE(flags & Subscription::act4Last);
            cnt++;
        }});
        box.activate();
        EXPECT_EQ(1, cnt);
    }

    //next/last, multiple
    {
        Box box;

        int cnt1=0;
        int cnt2=0;
        int cnt3=0;
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_TRUE(flags & Subscription::act4Next);
            cnt1++;
        }});
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_TRUE(flags & Subscription::act4Next);
            cnt2++;
        }});
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_TRUE(flags & Subscription::act4Last);
            cnt3++;
        }});
        box.activate();
        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(1, cnt2);
        EXPECT_EQ(1, cnt3);
        box.activate();
        EXPECT_EQ(2, cnt1);
        EXPECT_EQ(2, cnt2);
        EXPECT_EQ(2, cnt3);
    }

    //with context
    {
        Box box;

        int cnt=0;
        box.push(new Subs{[&](void* ctx, std::uint_fast8_t)
        {
            EXPECT_EQ(std::intptr_t(42), reinterpret_cast<std::intptr_t>(ctx));
            cnt++;
        }});
        box.activate(reinterpret_cast<void*>(42));
        EXPECT_EQ(1, cnt);
    }

    //del single
    {
        Box box;

        int cnt=0;
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_FALSE(flags & Subscription::del);
            cnt++;
        }});
        box.activate(nullptr, Box::del);
        EXPECT_EQ(1, cnt);

        box.activate(nullptr, Box::del);
        EXPECT_EQ(1, cnt);
    }

    //del multiple
    {
        Box box;

        int cnt1=0;
        int cnt2=0;
        int cnt3=0;
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_FALSE(flags & Subscription::del);
            cnt1++;
        }});
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_FALSE(flags & Subscription::del);
            cnt2++;
        }});
        box.push(new Subs{[&](void *, std::uint_fast8_t flags)
        {
            EXPECT_FALSE(flags & Subscription::del);
            cnt3++;
        }});
        box.activate(nullptr, Box::del);
        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(1, cnt2);
        EXPECT_EQ(1, cnt3);
        box.activate();
        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(1, cnt2);
        EXPECT_EQ(1, cnt3);
    }

    //recursive
    {
        Box box;

        int deep = 0;

        std::vector<Subscription *> subscriptions;

        std::function<void(void *, std::uint_fast8_t)> f = [&](void *, std::uint_fast8_t)
        {
            deep++;

            if(deep<8)
            {
                {
                    Subscription* s = subscriptions[0];
                    subscriptions.erase(subscriptions.begin());
                    box.removeAndDelete(s);
                }

                {
                    Subscription* s = new Subs{f};
                    box.push(s);
                    subscriptions.push_back(s);
                }

                box.activate(nullptr, Box::del);
            }

            deep--;
        };

        Subscription* s = new Subs{f};
        box.push(s);
        subscriptions.push_back(s);

        box.activate(nullptr, Box::del);
    }

    //recursive2
    {
        Box box;

        int deep = 0;

        std::function<void(void *, std::uint_fast8_t)> f1 = [&](void *, std::uint_fast8_t)
        {
        };


        std::function<void(void *, std::uint_fast8_t)> f2 = [&](void *, std::uint_fast8_t)
        {
            deep++;

            if(deep<2)
            {
                box.activate();
            }

            deep--;
        };

        box.push(new Subs{f1});
        box.push(new Subs{f2});
        box.push(new Subs{f1});
        box.activate();
    }
}
