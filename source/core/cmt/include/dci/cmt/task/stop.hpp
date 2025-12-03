// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>

namespace dci::cmt::task
{
    class Stop
        : public dci::exception::Skeleton<Stop, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<Stop, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0x04,0xd0,0xc8,0x71,0x07,0x23,0x43,0x03,0x9d,0xdb,0x7e,0xfd,0x0e,0xa0,0xc5,0xce};
    };
}
