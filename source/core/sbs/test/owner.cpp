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

        Subs(Owner& owner, const auto& f)
            : Subscription(&Subs::activator, &owner)
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
TEST(sbs, owner)
{
    //flush
    {
        Box box;

        Owner owner;
        int cnt=0;
        box.push(new Subs{owner, [&](void *, std::uint_fast8_t)
        {
            cnt++;
        }});
        box.activate();
        EXPECT_EQ(1, cnt);

        owner.flush();

        box.activate();
        EXPECT_EQ(1, cnt);

    }

    //destructor
    {
        Box box;

        Owner * owner = new Owner;
        int cnt=0;
        box.push(new Subs{*owner, [&](void *, std::uint_fast8_t)
        {
            cnt++;
        }});
        box.activate();
        EXPECT_EQ(1, cnt);

        delete owner;

        box.activate();
        EXPECT_EQ(1, cnt);

    }

}
