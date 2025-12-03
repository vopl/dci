// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"

namespace dci::cmt::impl
{
    class Barrier
        : public Waitable
    {
        Barrier(const Barrier&) = delete;
        void operator=(const Barrier&) = delete;

    public:
        Barrier(std::size_t depth);
        ~Barrier();
        static void tryDestruction(Barrier* b);

        bool canStride() const;
        bool tryStride();
        void stride();

    public:
        void wait();

    private:
        std::size_t _depth = 0;
    };
}
