// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

namespace dci::cmt::impl
{
    class Raisable
    {
        Raisable(const Raisable&) = delete;
        void operator=(const Raisable&) = delete;

    public:
        Raisable(void (* raise)(Raisable *));
        ~Raisable();
        static void tryDestruction(Raisable*);

        void raise();

    private:
        void (* _raise)(Raisable *) = nullptr;
    };
}
