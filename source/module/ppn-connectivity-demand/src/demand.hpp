// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "demand/registry.hpp"
#include "demand/element.hpp"

namespace dci::module::ppn::connectivity
{
    class Demand
        : public idl::gen::ppn::connectivity::Demand<>::Opposite
        , public host::module::ServiceBase<Demand>
    {
    public:
        Demand();
        ~Demand();

    private:
        bool _started {};
        demand::Registry _registry {this};
    };
}
