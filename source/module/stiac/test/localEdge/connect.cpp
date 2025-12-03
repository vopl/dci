// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../utils/bundle.hpp"
#include "test/victimInterface.hpp"

using namespace dci::idl::gen::stiac::test;
using namespace ::utils;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_stiac, localEdge_connect)
{
    ///////////////////////////
    // неиницаализированный протокол
    {
        Protocol<> p = testManager()->createService<Protocol<>>().value();
        EXPECT_EQ(protocol::State::null, p->state().value());
    }

    ///////////////////////////
    // инициализация
    {
        Bundle b(true, false);

        EXPECT_EQ(protocol::State::null, b._p1->state().value());
        EXPECT_EQ(protocol::State::null, b._p2->state().value());
    }

    ///////////////////////////
    // запуск
    {
        Bundle b;

        EXPECT_EQ(protocol::State::work, b._p1->state().value());
        EXPECT_EQ(protocol::State::work, b._p2->state().value());

        EXPECT_EQ(localEdge::State::work, b._l1->state().value());
        EXPECT_EQ(localEdge::State::work, b._l2->state().value());
    }

    ///////////////////////////
    // запуск и уничтожение локального порта
    {
        Bundle b;

        b._l1 = LocalEdge<>();
        EXPECT_EQ(protocol::State::pause, b._p1->state().value());
    }

    ///////////////////////////
    // запуск и уничтожение удаленного порта
    {
        Bundle b;

        b._r1 = RemoteEdge<>();
        EXPECT_EQ(protocol::State::pause, b._p1->state().value());
        EXPECT_EQ(localEdge::State::pause, b._l1->state().value());
    }

    ///////////////////////////
    // пустой интерфейс не должен приниматься локальным портом
    {
        Bundle b;
        EXPECT_THROW(b._l1->put(idl::Interface()).value(), localEdge::BadArgument);
    }

    ///////////////////////////
    // реакция на плохой бинарный поток
    {
        Bundle b;

        int scnt1 = 0;
        int scnt2 = 0;

        int ecnt1 = 0;
        int ecnt2 = 0;

        b._l1->stateChanged() += [&](localEdge::State s)
        {
            if(localEdge::State::fail == s)
            {
                scnt1++;
            }
        };

        b._l2->stateChanged() += [&](localEdge::State s)
        {
            if(localEdge::State::fail == s)
            {
                scnt2++;
            }
        };

        b._l1->failed() += [&](ExceptionPtr e)
        {
            try
            {
                std::rethrow_exception(e);
            }
            catch(const localEdge::BadInput&)
            {
                ecnt1++;
            }
        };

        b._l2->failed() += [&](ExceptionPtr e)
        {
            try
            {
                std::rethrow_exception(e);
            }
            catch(const localEdge::BadInput&)
            {
                ecnt2++;
            }
        };

        EXPECT_EQ(0, scnt1);
        EXPECT_EQ(0, scnt2);
        EXPECT_EQ(0, ecnt1);
        EXPECT_EQ(0, ecnt2);

        b._r1->input(Bytes("\x80\x81\x82\x83", 4));
        EXPECT_EQ(1, scnt1);
        EXPECT_EQ(0, scnt2);
        EXPECT_EQ(1, ecnt1);
        EXPECT_EQ(0, ecnt2);

        EXPECT_EQ(localEdge::State::fail, b._l1->state().value());
        EXPECT_EQ(localEdge::State::work, b._l2->state().value());

        EXPECT_EQ(protocol::State::fail, b._p1->state().value());
        EXPECT_EQ(protocol::State::work, b._p2->state().value());
    }

    ///////////////////////////
    // проброс интерфейса и его самоуничтожение
    {
        Bundle b;
        int cnt1 = 0;
        int cnt2 = 0;
        b._l2->got() += [&](idl::Interface i) -> Future<None>
        {
            EXPECT_EQ(0, cnt1);
            EXPECT_EQ(0, cnt2);

            EXPECT_TRUE(i);
            //EXPECT_TRUE(Victim<interface::Dir::opposite>(i));
            cnt1++;

            //i разрушается
            i = idl::Interface();

            return readyFuture(None{});
        };

        Victim i;
        i.init();

        i.involvedChanged() += [&](bool v)
        {
            if(!v)
            {
                cnt2++;
            }
        };

        EXPECT_EQ(0, cnt1);
        EXPECT_EQ(0, cnt2);

        b._l1->put(idl::Interface(i.opposite())).value();

        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(1, cnt2);
    }

    ///////////////////////////
    // проброс интерфейса и его самоуничтожение с другой стороны
    {
        Bundle b;
        int cnt1 = 0;
        int cnt2 = 0;

        idl::Interface holded2;

        b._l2->got() += [&](idl::Interface i) -> Future<None>
        {
            EXPECT_EQ(0, cnt1);
            EXPECT_EQ(0, cnt2);

            EXPECT_TRUE(i);
            Victim<>::Opposite(i).involvedChanged() += [&](bool v)
            {
                if(!v)
                {
                    cnt2++;
                }
            };

            cnt1++;

            holded2 = i;//чтобы не уничтожился

            return readyFuture(None{});
        };

        Victim i;
        i.init();

        EXPECT_EQ(0, cnt1);
        EXPECT_EQ(0, cnt2);

        b._l1->put(idl::Interface(i.opposite())).value();

        EXPECT_EQ(0, cnt2);
        i = Victim<>();

        EXPECT_EQ(1, cnt1);
        EXPECT_EQ(1, cnt2);
    }
}
