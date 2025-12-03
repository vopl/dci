// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"
#include "scopeEntry.hpp"
#include "decl.hpp"

#include <map>

namespace dci::idl::im::ast
{
    struct SScope : SScopeEntry
    {
        std::vector<Name> nestedNames;
        std::vector<Decl> decls;

        std::vector<SScope *>               siblings;
        std::multimap<std::string, Decl>    name2Decl;

        std::vector<SAlias *>               aliases;
        std::vector<SStruct *>              structs;
        std::vector<SEnum *>                enums;
        std::vector<SFlags *>               flagses;
        std::vector<SException *>           exceptions;
        std::vector<SInterface *>           interfaces;
        std::vector<SScope *>               scopes;
    };
}
