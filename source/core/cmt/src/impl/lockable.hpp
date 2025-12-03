// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include <dci/cmt/recursionMode.hpp>

namespace dci::cmt::impl
{
    class Lockable
        : public Waitable
    {
        Lockable(const Lockable&) = delete;
        void operator=(const Lockable&) = delete;

    public:
        Lockable(
                bool (* canLock)(const Waitable* lockableBase),
                bool (* tryLock)(Waitable* lockableBase),
                void (* unlock)(Lockable* lockable));
        ~Lockable();
        static void tryDestruction(Lockable*);

        bool canLock() const;
        bool tryLock();
        void lock();
        void unlock();

    private:
        void (* _unlock)(Lockable* self) = nullptr;
    };
}
