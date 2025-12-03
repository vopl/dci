// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, spawn)
{
    //простой
    {
        int progress = 0;
        spawn() += [&]{
            progress++;
        };
        executeReadyFibers();
        EXPECT_EQ(progress, 1);
    }


    {
        int progress = 0;
        spawn() += [&]{
            spawn() += [&]{
                progress++;
            };
            spawn() += [&]{
                spawn() += [&]{
                    spawn() += [&]{
                        progress++;
                    };
                    progress++;
                };
                progress++;
            };
            progress++;
        };
        spawn() += [&]{
            progress++;

            spawn() += [&]{
                progress++;
            };
            spawn() += [&]{
                progress++;
                spawn() += [&]{
                    progress++;
                };
                spawn() += [&]{
                    progress++;
                };
            };
        };
        executeReadyFibers();
        EXPECT_EQ(progress, 10);
    }

    //с промисом
    {
        auto f = spawnv<int>([](auto& p){
            p.resolveValue(42);
        });
        executeReadyFibers();
        EXPECT_TRUE(f.waitValue());
        EXPECT_EQ(f.value(), 42);
    }

    {
        auto f = spawnv<int>([](){
            return 42;
        });
        executeReadyFibers();
        EXPECT_TRUE(f.waitValue());
        EXPECT_EQ(f.value(), 42);
    }

    {
        auto f = spawnv<>([](Promise<char>& p){
            p.resolveValue('x');
        });
        executeReadyFibers();
        EXPECT_TRUE(f.waitValue());
        EXPECT_EQ(f.value(), 'x');
    }

    {
        auto f = spawnv<>([](){
            return 'x';
        });
        executeReadyFibers();
        EXPECT_TRUE(f.waitValue());
        EXPECT_EQ(f.value(), 'x');
    }
}
