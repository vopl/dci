// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../local.hpp"

namespace dci::module::ppn::discovery::local
{
    class Preset
        : public Local<Preset, api::Preset<>::Opposite>
    {
    public:
        Preset();
        ~Preset();

        void started();
        void declared(const Entry& entry);
    };
}
