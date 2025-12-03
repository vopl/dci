// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../interface/side.hpp"

namespace dci::idl::contract
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <interface::Side> class md> struct MdDescriptor
    {
        static constexpr bool _declared = false;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <template <interface::Side> class md>
    const MdDescriptor<md> mdDescriptor;
}
