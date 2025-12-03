// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"

namespace dci::idl::im::ast
{
    struct SInterfaceBase
    {
        ScopedName          scopedName;

        SInterface*         instance{nullptr};

        Sign                sign4Layout;
    };
}
