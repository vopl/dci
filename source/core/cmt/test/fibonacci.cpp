// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace byWait
{
    namespace
    {
        Future<uint64_t> fibonacci(uint64_t n)
        {
            if(n < 2)
            {
                return readyFuture(n);
            }

            Future<uint64_t> lhs = fibonacci(n-1);
            Future<uint64_t> rhs = fibonacci(n-2);

            return readyFuture(lhs.value() + rhs.value());
        }
    }

    TEST(cmt, fibonacciByWait)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0).value(), 0u);
            EXPECT_EQ(fibonacci(1).value(), 1u);
            EXPECT_EQ(fibonacci(2).value(), 1u);
            EXPECT_EQ(fibonacci(3).value(), 2u);
            EXPECT_EQ(fibonacci(4).value(), 3u);
            EXPECT_EQ(fibonacci(5).value(), 5u);
            EXPECT_EQ(fibonacci(6).value(), 8u);
            EXPECT_EQ(fibonacci(7).value(), 13u);
            EXPECT_EQ(fibonacci(8).value(), 21u);
            EXPECT_EQ(fibonacci(9).value(), 34u);
            EXPECT_EQ(fibonacci(10).value(), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace byContinuation
{
    namespace
    {
        Future<uint64_t> fibonacci(uint64_t n)
        {
            if(n < 2)
            {
                return readyFuture(n);
            }

            Future<uint64_t> lhs = fibonacci(n-1);
            Future<uint64_t> rhs = fibonacci(n-2);

            return whenAll(lhs, rhs).chain() += [=]() mutable
            {
                return lhs.value() + rhs.value();
            };
        }
    }

    TEST(cmt, fibonacciByContinuation)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0).value(), 0u);
            EXPECT_EQ(fibonacci(1).value(), 1u);
            EXPECT_EQ(fibonacci(2).value(), 1u);
            EXPECT_EQ(fibonacci(3).value(), 2u);
            EXPECT_EQ(fibonacci(4).value(), 3u);
            EXPECT_EQ(fibonacci(5).value(), 5u);
            EXPECT_EQ(fibonacci(6).value(), 8u);
            EXPECT_EQ(fibonacci(7).value(), 13u);
            EXPECT_EQ(fibonacci(8).value(), 21u);
            EXPECT_EQ(fibonacci(9).value(), 34u);
            EXPECT_EQ(fibonacci(10).value(), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace bySpawn1
{
    namespace
    {
        uint64_t fibonacci(uint64_t n)
        {
            if(n < 2)
            {
                return n;
            }

            Future<uint64_t> lhs = spawnv() += [=]{return fibonacci(n-1);};
            uint64_t rhs = fibonacci(n-2);

            return lhs.value() + rhs;
        }
    }

    TEST(cmt, fibonacciBySpawn1)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0), 0u);
            EXPECT_EQ(fibonacci(1), 1u);
            EXPECT_EQ(fibonacci(2), 1u);
            EXPECT_EQ(fibonacci(3), 2u);
            EXPECT_EQ(fibonacci(4), 3u);
            EXPECT_EQ(fibonacci(5), 5u);
            EXPECT_EQ(fibonacci(6), 8u);
            EXPECT_EQ(fibonacci(7), 13u);
            EXPECT_EQ(fibonacci(8), 21u);
            EXPECT_EQ(fibonacci(9), 34u);
            EXPECT_EQ(fibonacci(10), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace bySpawn2
{
    namespace
    {
        uint64_t fibonacci(uint64_t n)
        {
            if(n < 2)
            {
                return n;
            }

            Future<uint64_t> lhs = spawnv() += [=]{return fibonacci(n-1);};
            Future<uint64_t> rhs = spawnv() += [=]{return fibonacci(n-2);};

            return lhs.value() + rhs.value();
        }
    }

    TEST(cmt, fibonacciBySpawn2)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0), 0u);
            EXPECT_EQ(fibonacci(1), 1u);
            EXPECT_EQ(fibonacci(2), 1u);
            EXPECT_EQ(fibonacci(3), 2u);
            EXPECT_EQ(fibonacci(4), 3u);
            EXPECT_EQ(fibonacci(5), 5u);
            EXPECT_EQ(fibonacci(6), 8u);
            EXPECT_EQ(fibonacci(7), 13u);
            EXPECT_EQ(fibonacci(8), 21u);
            EXPECT_EQ(fibonacci(9), 34u);
            EXPECT_EQ(fibonacci(10), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace bySpawn2Continuation
{
    namespace
    {
        uint64_t fibonacci(uint64_t n)
        {
            if(n < 2)
            {
                return n;
            }

            Future<uint64_t> lhs = spawnv() += [=]{return fibonacci(n-1);};
            Future<uint64_t> rhs = spawnv() += [=]{return fibonacci(n-2);};

            return (whenAll(lhs, rhs).chain() += [=]() mutable
            {
                return lhs.value() + rhs.value();
            }).value();
        }
    }

    TEST(cmt, fibonacciBySpawn2Continuation)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0), 0u);
            EXPECT_EQ(fibonacci(1), 1u);
            EXPECT_EQ(fibonacci(2), 1u);
            EXPECT_EQ(fibonacci(3), 2u);
            EXPECT_EQ(fibonacci(4), 3u);
            EXPECT_EQ(fibonacci(5), 5u);
            EXPECT_EQ(fibonacci(6), 8u);
            EXPECT_EQ(fibonacci(7), 13u);
            EXPECT_EQ(fibonacci(8), 21u);
            EXPECT_EQ(fibonacci(9), 34u);
            EXPECT_EQ(fibonacci(10), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace bySpawn2ResultArg
{
    namespace
    {
        void fibonacci(uint64_t n, Promise<uint64_t>& result)
        {
            if(n < 2)
            {
                result.resolveValue(n);
                return;
            }

            Future<uint64_t> lhs = spawnv() += [=](Promise<uint64_t>& result){ fibonacci(n-1, result);};
            Future<uint64_t> rhs = spawnv() += [=](Promise<uint64_t>& result){ fibonacci(n-2, result);};

            result.resolveValue(lhs.value() + rhs.value());
        }

        uint64_t fibonacci(uint64_t n)
        {
            Promise<uint64_t> result;
            fibonacci(n, result);
            return result.future().value();
        }
    }


    TEST(cmt, fibonacciBySpawn2ResultArg)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0), 0u);
            EXPECT_EQ(fibonacci(1), 1u);
            EXPECT_EQ(fibonacci(2), 1u);
            EXPECT_EQ(fibonacci(3), 2u);
            EXPECT_EQ(fibonacci(4), 3u);
            EXPECT_EQ(fibonacci(5), 5u);
            EXPECT_EQ(fibonacci(6), 8u);
            EXPECT_EQ(fibonacci(7), 13u);
            EXPECT_EQ(fibonacci(8), 21u);
            EXPECT_EQ(fibonacci(9), 34u);
            EXPECT_EQ(fibonacci(10), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}

///////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace bySpawn2ResultArgContinuation
{
    namespace
    {
        void fibonacci(uint64_t n, Promise<uint64_t>&& result)
        {
            if(n < 2)
            {
                result.resolveValue(n);
                return;
            }

            Future<uint64_t> lhs = spawnv() += [=](Promise<uint64_t>&& result){ fibonacci(n-1, std::move(result));};
            Future<uint64_t> rhs = spawnv() += [=](Promise<uint64_t>&& result){ fibonacci(n-2, std::move(result));};

            whenAll(lhs, rhs).then() += [lhs, rhs, result=std::move(result)]() mutable
            {
                result.resolveValue(lhs.value() + rhs.value());
            };
        }

        uint64_t fibonacci(uint64_t n)
        {
            Promise<uint64_t> result;
            auto future = result.future();
            fibonacci(n, std::move(result));
            return future.value();
        }
    }

    TEST(cmt, fibonacciBySpawn2ResultArgContinuation)
    {
        bool allDone = false;
        spawn() += [&]
        {

            EXPECT_EQ(fibonacci(0), 0u);
            EXPECT_EQ(fibonacci(1), 1u);
            EXPECT_EQ(fibonacci(2), 1u);
            EXPECT_EQ(fibonacci(3), 2u);
            EXPECT_EQ(fibonacci(4), 3u);
            EXPECT_EQ(fibonacci(5), 5u);
            EXPECT_EQ(fibonacci(6), 8u);
            EXPECT_EQ(fibonacci(7), 13u);
            EXPECT_EQ(fibonacci(8), 21u);
            EXPECT_EQ(fibonacci(9), 34u);
            EXPECT_EQ(fibonacci(10), 55u);

            allDone = true;
        };

        executeReadyFibers();

        EXPECT_TRUE(allDone);
    }
}
