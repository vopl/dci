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
TEST(sbs, remove)
{
    //single
    {
        Box box;

        int cnt=0;
        Subs* subs = new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt++;
        }};
        box.push(subs);

        box.activate();
        EXPECT_EQ(1, cnt);

        box.removeAndDelete(subs);

        box.activate();
        EXPECT_EQ(1, cnt);
    }

    //single destroy
    {
        Box box;

        int cnt=0;
        Subs* subs = new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt++;
        }};
        box.push(subs);

        box.activate();
        EXPECT_EQ(1, cnt);

        delete subs;

        box.activate();
        EXPECT_EQ(1, cnt);
    }

    //multiple
    {
        Box box;

        int cnt1=0;
        int cnt2=0;
        Subs* subs1;
        Subs* subs2;

        subs1 = new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt1++;
            box.removeAndDelete(subs2);
        }};
        box.push(subs1);

        subs2 = new Subs{[&](void *, std::uint_fast8_t)
        {
            cnt2++;
        }};
        box.push(subs2);

        box.activate();
        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(0, cnt2);

        subs2 = subs1;
        box.activate();
        EXPECT_EQ(2, cnt1);
        EXPECT_EQ(0, cnt2);

        box.activate();
        EXPECT_EQ(2, cnt1);
        EXPECT_EQ(0, cnt2);
    }
}
