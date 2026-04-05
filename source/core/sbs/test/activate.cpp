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
