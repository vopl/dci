// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include "raisable.hpp"
#include <dci/cmt/wakeMode.hpp>

namespace dci::cmt::impl
{
    class Notifier
        : public Waitable
        , public Raisable
    {
        Notifier(const Notifier&) = delete;
        void operator=(const Notifier&) = delete;

    public:
        Notifier(WakeMode wakeMode);
        ~Notifier();
        static void tryDestruction(Notifier* n);

        bool isRaised() const;
        void reset();

    public:
        void wait();

    public:
        void raise();

    private:
        bool canAcquire() const;
        bool tryAcquire();

    private:
        WakeMode    _wakeMode   = WakeMode::all;
        bool        _raised     = false;
    };
}
