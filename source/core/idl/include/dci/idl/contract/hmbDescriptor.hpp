// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>

namespace dci::idl::contract
{
    struct HmDescriptorBase;

    struct HmbDescriptor
    {
        uint32                  _bundleOffset;
        const HmDescriptorBase* _hmd;
    };
}
