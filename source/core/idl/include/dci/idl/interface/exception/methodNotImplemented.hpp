// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>

namespace dci::idl::interface::exception
{
    class MethodNotImplemented
        : public dci::exception::Skeleton<MethodNotImplemented, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<MethodNotImplemented, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0xb2,0x2d,0x12,0xe1,0xf1,0xfb,0x43,0x43,0x8c,0x51,0x08,0xbb,0x05,0x96,0x90,0xbc};
    };
}
