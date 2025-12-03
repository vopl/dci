// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, scheduler)
{
    int progress = 0;

    progress = 0;
    spawn() += [&](){progress++;};
    executeReadyFibers();
    EXPECT_EQ(progress, 1);


    progress = 0;
    spawn() += [&](){progress++;};
    spawn() += [&](){yield();progress++;};
    spawn() += [&](){progress++;yield();};
    spawn() += [&](){progress++;};
    spawn() += [&](){yield();progress++;yield();};

    spawn() += [&](){
        yield();
        spawn() += [&](){progress++;yield();progress++;};
        yield();
        progress++;
    };

    executeReadyFibers();
    EXPECT_EQ(progress, 8);
}
