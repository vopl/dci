// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"
#include "../posInSources.hpp"
#include <optional>

namespace dci::idl::im::ast
{
    struct SName
    {
        PosInSources                pos;
        std::string                 value;
        std::optional<std::string>  value4Abi;
    };
}
