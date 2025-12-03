// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include "raisable.hpp"
#include "wakeMode.hpp"

namespace dci::cmt
{
    class API_DCI_CMT Pulser
        : public himpl::FaceLayout<Pulser, impl::Pulser, Waitable, Raisable>
    {
        Pulser(const Pulser&) = delete;
        void operator=(const Pulser&) = delete;

    public:
        Pulser(WakeMode wakeMode = WakeMode::all);
        ~Pulser();

    public:
        void wait();

    public:
        void raise();
    };
}
