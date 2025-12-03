// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::inproc
{
    class Connector
        : public api::Connector<>::Opposite
        , public host::module::ServiceBase<Connector>
    {
    public:
        Connector();
        ~Connector();
    };
}
