// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "lockable.hpp"
#include "recursionMode.hpp"

namespace dci::cmt
{
    class API_DCI_CMT Mutex
        : public himpl::FaceLayout<Mutex, impl::Mutex, Lockable>
    {
        Mutex(const Mutex&) = delete;
        void operator=(const Mutex&) = delete;

    public:
        Mutex(RecursionMode recursionMode = RecursionMode::nonRecursive);
        ~Mutex();

    public:
        bool canLock() const;
        bool tryLock();
        void lock();
        void unlock();

    public:
        void wait();
    };
}
