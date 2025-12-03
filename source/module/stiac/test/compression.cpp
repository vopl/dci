// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "utils/bundle.hpp"
#include "test/victimInterface.hpp"

using namespace dci::idl::gen::stiac::test;

namespace
{
    struct Bundle
        : public ::utils::Bundle
    {
        Victim<>            _i1;
        Victim<>::Opposite  _i2;

        bool _fail1 = false;
        bool _fail2 = false;

        Bundle()
            : ::utils::Bundle(false, false)
        {
            _inputRequirements = protocol::Requirements::compression;
            _outputRequirements = protocol::Requirements::compression;

            init();

            _l2->failed() += [&](ExceptionPtr e)
            {
                _fail2 = true;

                try
                {
                    std::rethrow_exception(e);
                }
                catch(const std::exception& e)
                {
                    std::cout<<"fail 2: "<<e.what()<<std::endl;
                }
            };

            _l1->failed() += [&](ExceptionPtr e)
            {
                _fail1 = true;

                try
                {
                    std::rethrow_exception(e);
                }
                catch(const std::exception& e)
                {
                    std::cout<<"fail 1: "<<e.what()<<std::endl;
                }
            };

            start();

            _l2->got() += [&](idl::Interface&& i)
            {
                _i2 = i;
                return readyFuture(None{});
            };


            _l1->put(idl::Interface(_i1.init2())).value();
        }
    };


    String resBuilder(String s, bool b)
    {
        return "pre_" + s + "_mid_" + (b ? "true" : "false") + "_post";
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_stiac, compression)
{
    Bundle b;

    b._i1->out_m1() += [](String s, bool b)
    {
        return readyFuture(resBuilder(s, b));
    };

    for(int k(0); k<20; k+=2)
    {
        for(int k2(0); k2<20; k2+=3)
        {
            std::string content = "content_"+std::to_string(k2)+"_"+std::string(static_cast<size_t>(k*k2*10+10000), '.');
            EXPECT_TRUE(resBuilder(content, (k+k2)%2) == b._i2->out_m1(content, (k+k2)%2).value());
        }
    }

    EXPECT_FALSE(b._fail1);
    EXPECT_FALSE(b._fail2);
}
