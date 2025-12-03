// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, futureCancel)
{
    bool bodyComplete = false;
    spawn() += [&]()
    {
        //не отменено
        {
            Promise<void> p1;
            EXPECT_FALSE(p1.resolvedCancel());
            EXPECT_FALSE(p1.future().resolvedCancel());
        }

        //отмена при разрушении всех футур
        {
            Promise<void> p1;

            {
                Future<void> f1 = p1.future();
                EXPECT_FALSE(p1.resolvedCancel());
                EXPECT_FALSE(f1.resolvedCancel());

                Future<void> f2 = p1.future();
                EXPECT_FALSE(p1.resolvedCancel());
                EXPECT_FALSE(f2.resolvedCancel());
            }

            EXPECT_TRUE(p1.resolvedCancel());
            EXPECT_TRUE(p1.future().resolvedCancel());
        }

        //отмена при разрушении промиса
        {
            Future<void> f1;
            {
                Promise<void> p1;
                f1 = p1.future();

                EXPECT_FALSE(f1.resolvedCancel());
            }

            EXPECT_TRUE(f1.resolvedCancel());
        }

        //явная отмена из футуры
        {
            Promise<void> p1;
            Future<void> f1 = p1.future();

            f1.resolveCancel();
            EXPECT_TRUE(p1.resolvedCancel());
            EXPECT_TRUE(f1.resolvedCancel());
        }

        //явная отмена из промиса
        {
            Promise<void> p1;
            Future<void> f1 = p1.future();

            p1.resolveCancel();
            EXPECT_TRUE(p1.resolvedCancel());
            EXPECT_TRUE(f1.resolvedCancel());
        }

        //повторная отмена из футуры
        {
            Promise<void> p1;
            Future<void> f1 = p1.future();

            EXPECT_TRUE(f1.resolveCancel());
            EXPECT_FALSE(f1.resolveCancel());
            EXPECT_FALSE(p1.resolveCancel());
        }

        //повторная отмена из промиса
        {
            Promise<void> p1;
            EXPECT_TRUE(p1.resolveCancel());
            EXPECT_FALSE(p1.resolveCancel());
            EXPECT_FALSE(p1.future().resolveCancel());
        }

        //нотификация промиса об отмене
        {
            Promise<void> p1;

            int cnt = 0;
            p1.canceled() += [&]
            {
                cnt++;
            };

            EXPECT_EQ(cnt, 0);
            p1.resolveCancel();
            EXPECT_EQ(cnt, 1);
        }
        {
            Promise<void> p1;

            int cnt = 0;
            p1.canceled() += [&]
            {
                cnt+=1;
            };
            p1.canceled() += [&]
            {
                cnt+=10;
            };

            EXPECT_EQ(cnt, 0);
            p1.resolveCancel();
            EXPECT_EQ(cnt, 11);
        }

        //нотификация футуры об отмене
        {
            {
                Promise<void> p1;
                Future<void> f1 = p1.future();

                int cnt = 0;
                f1.then() += [&](Future<void> in)
                {
                    EXPECT_TRUE(in.resolved());
                    EXPECT_FALSE(in.resolvedValue());
                    EXPECT_FALSE(in.resolvedException());
                    EXPECT_TRUE(in.resolvedCancel());

                    EXPECT_THROW(in.value(), future::Exception);

                    cnt+=1;
                };
                f1.then() += [&]()
                {
                    cnt+=10;
                };

                EXPECT_EQ(cnt, 0);
                p1.resolveCancel();
                EXPECT_EQ(cnt, 11);
            }
        }

        //отмена при chain
        {
            {
                Promise<void> p1;
                Future<void> f1 = p1.future();

                int cnt = 0;
                auto f2 = f1.chain() += [&](Future<void> /*in*/, Promise<int>& /*out*/)
                {
                    cnt+=1;
                };

                EXPECT_EQ(cnt, 0);
                f2.resolveCancel();
                EXPECT_EQ(cnt, 0);
                EXPECT_TRUE(p1.resolvedCancel());
            }
        }

        bodyComplete = true;
    };
    executeReadyFibers();
    EXPECT_TRUE(bodyComplete);
}
