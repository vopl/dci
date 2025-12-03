// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../datagramBased.hpp"

namespace dci::module::ppn::discovery::local::datagramBased
{
    class MachineScope
        : public DatagramBased<MachineScope, api::MachineScope<>::Opposite>
    {
        using Base = DatagramBased<MachineScope, api::MachineScope<>::Opposite>;

    public:
        MachineScope(host::Manager* hostManager);
        ~MachineScope();

        static const std::string_view& tag();

        const net::Endpoint& masterPoint() const;
        const net::Endpoint& slavePoint() const;

        void tick();

        bool addressAllowed(const transport::Address& a);
    };
}
