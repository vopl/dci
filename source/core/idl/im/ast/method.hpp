// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"
#include "typeUse.hpp"

namespace dci::idl::im::ast
{
    enum class MethodDirection
    {
        in,
        out
    };

    struct SMethod
    {
        MethodDirection             direction{MethodDirection::out};
        Name                        name;
        std::vector<MethodParam>    query;
        bool                        noreply{false};
        TypeUse                     reply;

        SInterface*                 owner{nullptr};

        Sign                        sign4Layout;
    };
}
