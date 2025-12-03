// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>

namespace dci::aup
{
    class Exception
        : public dci::exception::Skeleton<Exception, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<Exception, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0x60,0x6a,0xca,0x96,0x0b,0x46,0x45,0x8f,0x9e,0x51,0xca,0xbb,0xf5,0xb6,0x68,0xbb};
    };
}
