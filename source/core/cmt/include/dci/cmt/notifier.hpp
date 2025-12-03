// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include "raisable.hpp"
#include "wakeMode.hpp"

namespace dci::cmt
{
    class API_DCI_CMT Notifier
        : public himpl::FaceLayout<Notifier, impl::Notifier, Waitable, Raisable>
    {
        Notifier(const Notifier&) = delete;
        void operator=(const Notifier&) = delete;

    public:
        Notifier(WakeMode wakeMode = WakeMode::all);
        ~Notifier();

        bool isRaised() const;
        void reset();

    public:
        void wait();

    public:
        void raise();
    };
}
