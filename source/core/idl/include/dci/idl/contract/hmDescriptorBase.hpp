// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "hmbDescriptor.hpp"

namespace dci::idl::contract
{
    struct alignas(HmbDescriptor) HmDescriptorBase
    {
        constexpr HmDescriptorBase(bool mdiSideSame)
            : _mdiSideSame(mdiSideSame)
        {
        }

        constexpr const HmbDescriptor& hmbd(uint32 index) const
        {
            return _hmbdFake[1+index];
        }

        union
        {
            const HmbDescriptor _hmbdFake[1];
            const bool _mdiSideSame;
        };

        //const HmbDescriptor _hmbd[N];
    };
}
