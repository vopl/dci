// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/poll.hpp>
#include <dci/cmt.hpp>
#include "utils/complexRun.hpp"

using namespace dci::poll;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace
{
    int scopeRaiiCounter = 0;
    struct ScopeRaii
    {
        ScopeRaii(){scopeRaiiCounter++;}
        ~ScopeRaii(){scopeRaiiCounter--;}
    };
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(poll, waitableTimer)
{
    //single
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        WaitableTimer t1{std::chrono::milliseconds{1}};
        WaitableTimer t2{std::chrono::milliseconds{1}};

        t1.start();
        t2.start();

        t1.wait();
        t2.wait();
    };

    //mass
    dci::cmt::spawn() += []
    {
        ScopeRaii sr;

        WaitableTimer t1{std::chrono::milliseconds{1}};
        WaitableTimer t2{std::chrono::milliseconds{1}};

        t1.start();
        t2.start();

        dci::cmt::waitAll(t1, t2);
    };

    utils::complexRun();

    EXPECT_EQ(scopeRaiiCounter, 0);
}
