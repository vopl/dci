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
