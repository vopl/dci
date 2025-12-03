// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"
#include "../posInSources.hpp"
#include "name.hpp"
#include <boost/variant.hpp>

namespace dci::idl::im::ast
{
    struct SScopeEntry;

    struct SScopedName
    {
        PosInSources        pos;
        bool                root{false};
        std::vector<Name>   values;

        boost::variant<
              SAlias *
            , SStruct *
            , SEnum *
            , SFlags *
            , SException *
            , SInterface *
        >                   asDecl;
        SScopeEntry *       asScopedEntry {nullptr};

        std::string toString(const std::string& delim = "::") const;
        ScopedName toFullScopedName() const;
    };
}
