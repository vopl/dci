// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "lockable.hpp"
#include <dci/cmt/recursionMode.hpp>
#include "ctx/fiber.hpp"

namespace dci::cmt::impl
{
    class Mutex
        : public Lockable
    {
        Mutex(const Mutex&) = delete;
        void operator=(const Mutex&) = delete;

    public:
        Mutex(RecursionMode recursionMode);
        ~Mutex();
        static void tryDestruction(Mutex* m);

    public:
        bool canLock() const;
        bool tryLock();
        void lock();
        void unlock();

    public:
        void wait();

    protected:
        const RecursionMode _recursionMode = RecursionMode::nonRecursive;
        ctx::Fiber* _owner = nullptr;
        std::size_t _counter = 0;
    };
}
