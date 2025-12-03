// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/poll.hpp>
#include <dci/cmt.hpp>

using namespace dci::poll;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(poll, instance)
{
    std::error_code ec;

    {
        ec = deinitialize();
        EXPECT_TRUE(ec);

        ec = run();
        EXPECT_TRUE(ec);

        ec = stop();
        EXPECT_TRUE(ec);
    }

    {
        ec = initialize();
        EXPECT_FALSE(ec);

        ec = deinitialize();
        EXPECT_FALSE(ec);
    }

    {
        ec = initialize();
        EXPECT_FALSE(ec);

        ec = initialize();
        EXPECT_TRUE(ec);

        ec = deinitialize();
        EXPECT_FALSE(ec);

        ec = deinitialize();
        EXPECT_TRUE(ec);
    }

    {
        ec = run();
        EXPECT_TRUE(ec);

        ec = stop();
        EXPECT_TRUE(ec);

        ec = deinitialize();
        EXPECT_TRUE(ec);
    }

    {
        ec = initialize();
        EXPECT_FALSE(ec);

        dci::cmt::spawn() += []{
            auto ec = stop();
            EXPECT_FALSE(ec);
        };

        {
            dci::sbs::Owner doSomeWorkOwner;
            dci::poll::doSomeWork() += doSomeWorkOwner * [&]
            {
                return dci::cmt::executeReadyFibers();
            };
            ec = run();
            EXPECT_FALSE(ec);
        }

        ec = deinitialize();
        EXPECT_FALSE(ec);
    }

    {
        ec = initialize();
        EXPECT_FALSE(ec);

        ec = run();
        EXPECT_FALSE(ec);

        ec = deinitialize();
        EXPECT_FALSE(ec);
    }
}
