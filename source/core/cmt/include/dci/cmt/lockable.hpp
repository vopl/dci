// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"

namespace dci::cmt
{
    class API_DCI_CMT Lockable
        : public himpl::FaceLayout<Lockable, impl::Lockable, Waitable>
    {
        Lockable(const Lockable&) = delete;
        void operator=(const Lockable&) = delete;

    protected:
        DCI_INTEGRATION_APIDECL_LOCAL Lockable(himpl::FakeConstructionArg);
        Lockable() = delete;
        DCI_INTEGRATION_APIDECL_LOCAL ~Lockable();

    public:
        bool canLock() const;
        bool tryLock();
        void lock();
        void unlock();
    };
}
