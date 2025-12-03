// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "lockable.hpp"
#include "recursionMode.hpp"

namespace dci::cmt
{
    class API_DCI_CMT Semaphore
        : public himpl::FaceLayout<Semaphore, impl::Semaphore, Lockable>
    {
        Semaphore(const Semaphore&) = delete;
        void operator=(const Semaphore&) = delete;

    public:
        Semaphore(std::size_t depth);
        ~Semaphore();

    public:
        bool canLock() const;
        bool tryLock();
        void lock();
        void unlock();

    public:
        void wait();
    };
}
