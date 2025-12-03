// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "lockable.hpp"
#include "ctx/fiber.hpp"
#include <dci/cmt/recursionMode.hpp>
#include <cstdint>

namespace dci::cmt::impl
{
    class Semaphore
        : public Lockable
    {
        Semaphore(const Semaphore&) = delete;
        void operator=(const Semaphore&) = delete;

    public:
        Semaphore(std::size_t depth);
        ~Semaphore();
        void tryDestruction(Semaphore* s);

    public:
        bool canLock() const;
        bool tryLock();
        void lock();
        void unlock();

    public:
        void wait();

    private:
        std::size_t _depth;
        std::size_t _counter = 0;
    };
}
