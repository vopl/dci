// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include "raisable.hpp"
#include <dci/cmt/wakeMode.hpp>

namespace dci::cmt::impl
{
    class Pulser
        : public Waitable
        , public Raisable
    {
        Pulser(const Pulser&) = delete;
        void operator=(const Pulser&) = delete;

    public:
        Pulser(WakeMode wakeMode);
        ~Pulser();
        static void tryDestruction(Pulser* p);

    public:
        void wait();

    public:
        void raise();

    private:
        bool canAcquire() const;
        bool tryAcquire();

    private:
        WakeMode    _wakeMode   = WakeMode::all;
    };
}
