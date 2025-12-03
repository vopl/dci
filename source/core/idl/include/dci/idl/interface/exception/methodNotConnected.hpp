// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>

namespace dci::idl::interface::exception
{
    class MethodNotConnected
        : public dci::exception::Skeleton<MethodNotConnected, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<MethodNotConnected, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0x7f,0x8a,0x2b,0xb3,0x49,0xf6,0x40,0x2f,0xbc,0xb6,0x49,0xf9,0xa6,0x36,0x30,0x41};
    };
}
