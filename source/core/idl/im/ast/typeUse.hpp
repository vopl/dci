// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"
#include <boost/variant.hpp>

namespace dci::idl::im::ast
{
    using TypeUse = boost::variant<
          Primitive
        , Tuple
        , List
        , Set
        , Map
        , Ptr
        , Opt
        , Array
        , Variant
        , ScopedName
    >;
}
