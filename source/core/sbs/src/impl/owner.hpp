// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

namespace dci::sbs::impl
{
    class Subscription;

    class Owner final
    {
    public:
        Owner();
        ~Owner();

        void flush();

    public:
        void push(Subscription* subscription);
        void remove(Subscription* subscription);

    private:
        Subscription* _first = nullptr;
        Subscription* _last = nullptr;
    };
}
