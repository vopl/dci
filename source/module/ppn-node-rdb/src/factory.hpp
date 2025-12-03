// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node::rdb
{
    class Factory
        : public api::Factory<>::Opposite
        , public host::module::ServiceBase<Factory>
    {
    public:
        Factory();
        ~Factory();
    };
}
