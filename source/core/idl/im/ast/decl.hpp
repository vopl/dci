// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"
#include <boost/variant.hpp>

namespace dci::idl::im::ast
{
    using Decl = boost::variant<
          Alias
        , Struct
        , Enum
        , Flags
        , Exception
        , Interface
        , Scope
    >;
}
