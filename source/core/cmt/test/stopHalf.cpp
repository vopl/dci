// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, stopHalf)
{
    spawn() += []
    {
        Event e1, e2;
        Event done1, done2;
        task::Owner to;

        e1.raise();

        spawn() += to * [&]
        {
            try
            {
                waitAll(e1, e2);
            }
            catch(...)
            {
            }

            done1.raise();
        };

        spawn() += [&]
        {
            to.stop();
            done2.raise();
        };

        done1.wait();
        done2.wait();
    };

    executeReadyFibers();
}
