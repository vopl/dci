// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>

namespace dci::sbs::exception
{
    class WireNotConnected
        : public dci::exception::Skeleton<WireNotConnected, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<WireNotConnected, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0xd7,0x24,0x41,0xd0,0x0c,0x0c,0x41,0x44,0x87,0x44,0x1e,0xae,0x96,0x6c,0x30,0x60};
    };
}
