// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/cmt.hpp>

using namespace dci::cmt;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(cmt, future)
{
    bool bodyComplete = false;
    spawn() += [&]()
    {
        //пустая без значения
        {
            Promise<> p;
            Future<> f = p.future();

            EXPECT_FALSE(f.resolved());
        }

        //резолв в значение
        {
            Promise<int> p;
            Future<int> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                f.wait();
                n.raise();
            };

            p.resolveValue(42);
            n.wait();

            EXPECT_TRUE(f.resolved());
            EXPECT_TRUE(f.waitValue());
            EXPECT_FALSE(f.waitException());

            EXPECT_EQ(f.value(), 42);
        }

        //резолв в ошибку
        {
            Promise<> p;
            Future<> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                f.wait();
                n.raise();
            };

            p.resolveException(std::make_exception_ptr(666));

            n.wait();

            EXPECT_TRUE(f.resolved());
            EXPECT_FALSE(f.waitValue());
            EXPECT_TRUE(f.waitException());

            EXPECT_THROW(f.value(), int);
            EXPECT_THROW(std::rethrow_exception(f.exception()), int);
        }

        //ожидание значения на геттерах
        {
            Promise<int> p;
            Future<int> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                EXPECT_EQ(f.value(), 42);
                n.raise();
            };

            spawn() += [&]()
            {
                p.resolveValue(42);
            };

            n.wait();
        }

        //ожидание исключения на геттерах
        {
            Promise<int> p;
            Future<int> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                EXPECT_THROW(f.value(), int);
                EXPECT_THROW(f.value(), int);
                EXPECT_THROW(std::rethrow_exception(f.exception()), int);
                n.raise();
            };

            spawn() += [&]()
            {
                p.resolveException(std::make_exception_ptr(666));
            };

            n.wait();
        }

        //множественное ожидание
        {
            Promise<int> p;
            Future<int> f = p.future();

            Event e1;
            Event e2;

            int processed = 0;
            spawn() += [&]()
            {
                f.wait();
                e1.wait();
                EXPECT_EQ(f.value(), 42);
                e2.wait();
                ++processed;
            };

            spawn() += [&]()
            {
                f.value();
                e1.wait();
                EXPECT_EQ(f.value(), 42);
                e2.wait();
                ++processed;
            };

            spawn() += [&]()
            {
                f.value();
                e1.wait();
                EXPECT_EQ(f.value(), 42);
                e2.wait();
                ++processed;
            };

            spawn() += [&]()
            {
                p.resolveValue(42);
            };

            e1.raise();
            e2.raise();

            while(processed < 3)
            {
                yield();
            }
        }

        //резолв значения через аргументы
        {
            Promise<int> p;
            Future<int> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                EXPECT_EQ(f.value(), 42);
                n.raise();
            };

            spawn() += [&]()
            {
                p.resolveValue(42);
            };

            n.wait();
        }

        //резолв исключения через перемещение
        {
            Promise<int> p;
            Future<int> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                EXPECT_THROW(f.value(), int);
                n.raise();
            };

            spawn() += [&]()
            {
                p.resolveException(std::make_exception_ptr(666));
            };

            n.wait();
        }

        //резолв исключения через копирование
        {
            Promise<int> p;
            Future<int> f = p.future();

            Notifier n;

            spawn() += [&]()
            {
                EXPECT_THROW(f.value(), int);
                n.raise();
            };

            spawn() += [&]()
            {
                auto e = std::make_exception_ptr(666);
                p.resolveException(e);
            };

            n.wait();
        }

        //перемещение значения
        {
            Promise<int> p;
            Future<int> f = p.future();

            p.resolveValue(42);

            int i = -1;

            auto mys = [](auto&& a, auto&& b)
            {
                auto k = std::move(a);
                a = std::move(b);
                b = std::move(k);
            };
            mys(f.detachValue(), std::move(i));

            EXPECT_EQ(f.detachValue(), -1);
            EXPECT_EQ(i, 42);
        }

        //перемещение исключения
        {
            Promise<int> p;
            Future<int> f = p.future();

            p.resolveException(std::make_exception_ptr(666));

            std::exception_ptr e = std::make_exception_ptr('6');

            auto mys = [](auto&& a, auto&& b)
            {
                auto k = std::move(a);
                a = std::move(b);
                b = std::move(k);
            };
            mys(f.detachException(), e);

            EXPECT_THROW(std::rethrow_exception(f.exception()), char);
            EXPECT_THROW(std::rethrow_exception(e), int);
        }

        //копия конструктором
        {
            Promise<int> p;
            p.resolveValue(42);

            Future<int> f1 = p.future();
            Future<int> f2(f1);

            EXPECT_EQ(f1.value(), 42);
            EXPECT_EQ(f2.value(), 42);
        }

        //копия оператором
        {
            Promise<int> p;
            p.resolveValue(42);

            Future<int> f1 = p.future();
            Future<int> f2;
            f2 = f1;

            EXPECT_EQ(f1.value(), 42);
            EXPECT_EQ(f2.value(), 42);
        }

        //перемещение конструктором
        {
            Promise<int> p;
            p.resolveValue(42);

            Future<int> f1 = p.future();
            Future<int> f2(std::move(f1));

            EXPECT_FALSE(f1.charged());
            EXPECT_EQ(f2.value(), 42);
        }

        //перемещение оператором
        {
            Promise<int> p;
            p.resolveValue(42);

            Future<int> f1 = p.future();
            Future<int> f2;
            f2 = std::move(f1);

            EXPECT_FALSE(f1.charged());
            EXPECT_EQ(f2.value(), 42);
        }

        //разрушение значения при разрушении футуры
        {
            struct Value
            {
                int& _progress;

                Value(int& progress)
                    : _progress(progress)
                {
                    _progress++;
                }

                Value(const Value& v)
                    : _progress(v._progress)
                {
                    _progress++;
                }

                ~Value()
                {
                    _progress--;
                }
            };

            int progress = 0;

            progress = 42;
            {
                Promise<Value> p;
                p.resolveValue(Value{progress});
            }
            EXPECT_EQ(progress, 42);

            progress = 42;
            {
                Promise<Value> p;
                p.resolveValue(Value{progress});
                Future<Value> f = p.future();
            }
            EXPECT_EQ(progress, 42);
        }

        //then вызов сразу
        {
            int progress = 0;
            Promise<int> p;

            p.resolveValue(42);
            p.future().then() += [&]()
            {
                progress++;
            };

            EXPECT_EQ(progress, 1);
        }

        //then вызов после резолва
        {
            int progress = 0;
            Promise<int> p;

            Future<> f2 = p.future().chain() += [&]()
            {
                progress++;
            };

            p.resolveValue(42);

            f2.wait();

            EXPECT_EQ(progress, 1);
        }

        //then с результатом
        {
            Promise<int> p;
            p.resolveValue(42);
            Future<int> f =p.future();

            Future<short> res;

            res = f.chain<short>([&]()
            {
                return short(43);
            });
            EXPECT_EQ(res.value(), 43);


            res = f.chain<short>([&](Promise<short>& res)
            {
                res.resolveValue(short{43});
            });
            EXPECT_EQ(res.value(), 43);


            res = f.chain<short>([&]()
            {
                throw std::string("exception");
                return short(0);
            });
            EXPECT_THROW(res.value(), std::string);

            res = f.chain<short>([&](Promise<short>& res)
            {
                res.resolveException(std::make_exception_ptr(std::string("exception")));
            });
            EXPECT_THROW(res.value(), std::string);
        }

        //then с результатом, автодетект типа результата
        {
            Promise<int> p;
            p.resolveValue(42);
            Future<int> f =p.future();

            Future<short> res;

            res = f.chain() += []() -> short
            {
                return short(43);
            };
            EXPECT_EQ(res.value(), 43);

            res = f.chain() += [&](Promise<short>& res)
            {
                res.resolveValue(short{43});
            };
            EXPECT_EQ(res.value(), 43);
        }

        bodyComplete = true;
    };
    executeReadyFibers();
    EXPECT_TRUE(bodyComplete);
}
