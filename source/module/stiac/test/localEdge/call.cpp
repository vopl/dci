// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../utils/bundle.hpp"
#include "test/victimInterface.hpp"

using namespace dci::idl::gen::stiac::test;

namespace
{
    struct Bundle : public ::utils::Bundle
    {
        Victim<>            _i1;
        Victim<>::Opposite  _i2;

        Bundle()
            : ::utils::Bundle()
        {
            _l2->got() += [&](idl::Interface&& i)
            {
                _i2 = i;
                return readyFuture(None{});
            };


            _l1->put(idl::Interface(_i1.init2())).value();
        }
    };
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_stiac, localEdge_call)
{
    //вызовы
    {
        Bundle b;

        ////////////////////////////////////////////
        {
            b._i2->in_m1() += [](uint32 arg)
            {
                EXPECT_EQ(42u, arg);
                return readyFuture<uint64>(43u);
            };

            EXPECT_EQ(43u, b._i1->in_m1(42u).value());
        }

        ////////////////////////////////////////////
        {
            b._i1->out_m1() += [](String arg1, bool_ arg2)
            {
                EXPECT_EQ(String("forty two"), arg1);
                EXPECT_EQ(true, arg2);

                return readyFuture<String>("forty three");
            };

            EXPECT_EQ(String("forty three"), b._i2->out_m1("forty two", true).value());
        }

        ////////////////////////////////////////////
        {
            AsArgument<>            asArgFwd;
            AsArgument<>::Opposite  asArgBwd;

            b._i2->in_m2() += [&](AsArgument<> arg)
            {
                EXPECT_TRUE(arg);
                asArgFwd = arg;
            };

            b._i1->in_m2(asArgBwd.init2());

            //вызовы по интерфейсу, проброшеному сквозь другой интерфейс
            {
                int cntFwd = 0;
                int cntBwd = 0;
                asArgFwd->out_m() += [&]
                {
                    cntBwd++;
                };
                asArgBwd->in_m() += [&]
                {
                    cntFwd++;
                };

                EXPECT_EQ(0, cntFwd);
                EXPECT_EQ(0, cntBwd);

                asArgFwd->in_m();
                EXPECT_EQ(1, cntFwd);

                asArgFwd->in_m();
                EXPECT_EQ(2, cntFwd);

                asArgBwd->out_m();
                EXPECT_EQ(1, cntBwd);

                asArgFwd->in_m();
                EXPECT_EQ(3, cntFwd);

                asArgBwd->out_m();
                EXPECT_EQ(2, cntBwd);
            }

            //разрушение
            {
                int cnt = 0;
                asArgFwd.involvedChanged() += [&](bool v)
                {
                    if(!v)
                    {
                        cnt++;
                    }
                };
                EXPECT_EQ(0, cnt);

                asArgBwd = idl::Interface();
                EXPECT_EQ(1, cnt);
            }
        }
    }

    //отложенный результат
    {
        Bundle b;

        Future<uint32> f;
        Promise<uint32> p;

        b._i2->in_withResult2() += [&]()
        {
            return p.future();
        };

        f = b._i1->in_withResult2();

        EXPECT_FALSE(f.resolved());
        EXPECT_FALSE(p.resolved());

        p.resolveValue(uint32{42});
        EXPECT_TRUE(f.resolvedValue());
        EXPECT_EQ(42u, f.value());
    }

    //разрушение отложенного результата
    {
        Bundle b;

        Future<uint32> f;
        Promise<uint32> p;

        b._i2->in_withResult2() += [&]()
        {
            return p.future();
        };

        f = b._i1->in_withResult2();

        EXPECT_FALSE(f.resolved());
        EXPECT_FALSE(p.resolved());

        p = Promise<uint32>();
        EXPECT_TRUE(f.resolvedCancel());
    }

    //разрушение отложенного результата
    {
        Bundle b;

        Future<uint32> f;
        Promise<uint32> p;

        b._i2->in_withResult2() += [&]()
        {
            return p.future();
        };

        f = b._i1->in_withResult2();

        EXPECT_FALSE(f.resolved());
        EXPECT_FALSE(p.resolved());

        f = Future<uint32>();
        EXPECT_TRUE(p.resolvedCancel());
    }

    //встречная коллизия
    {
        Bundle b;

        int fails = 0;

        b._p1->failed() += [&](ExceptionPtr e)
        {
            fails++;
            try
            {
                std::rethrow_exception(e);
            }
            catch(std::exception& e)
            {
                std::cout<<"p1 fail: "<<e.what()<<std::endl;
            }
        };

        b._p2->failed() += [&](ExceptionPtr e)
        {
            fails++;
            try
            {
                std::rethrow_exception(e);
            }
            catch(std::exception& e)
            {
                std::cout<<"p2 fail: "<<e.what()<<std::endl;
            }
        };

        b._p1->setAutoPumping(idl::gen::stiac::protocol::AutoPumping::none);
        b._p2->setAutoPumping(idl::gen::stiac::protocol::AutoPumping::none);

        b._p1->start();
        b._p2->start();

        Future<uint32> f;
        Promise<uint32> p;

        b._i2->in_withResult2() += [&]()
        {
            return p.future();
        };


        f = b._i1->in_withResult2();

        EXPECT_FALSE(f.resolved());
        EXPECT_FALSE(p.resolved());

        b._p2->pump();
        b._p1->pump();
        b._p2->pump();
        b._p1->pump();

        EXPECT_FALSE(f.resolved());
        EXPECT_FALSE(p.resolved());

        p.resolveValue(uint32{42});//коллизия 1

        EXPECT_FALSE(f.resolved());
        EXPECT_TRUE(p.resolved());

        f.resolveCancel();//коллизия 2

        EXPECT_TRUE(f.resolved());
        EXPECT_TRUE(p.resolved());

        b._p2->pump();
        b._p1->pump();
        b._p2->pump();
        b._p1->pump();

        EXPECT_EQ(0, fails);
    }
}
