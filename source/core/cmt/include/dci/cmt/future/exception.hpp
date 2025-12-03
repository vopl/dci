// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/exception.hpp>

namespace dci::cmt::future
{
    class Exception
        : public dci::exception::Skeleton<Exception, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<Exception, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0x10,0xd0,0x3b,0x22,0x01,0x57,0x44,0xf0,0x92,0x7a,0x5d,0xd1,0xdf,0xce,0x15,0x80};
    };
}
