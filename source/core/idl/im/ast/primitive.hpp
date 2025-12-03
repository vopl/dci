// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "predecl.hpp"

namespace dci::idl::im::ast
{
    enum class PrimitiveKind
    {
        none,

        bool_,

        int8,
        int16,
        int32,
        int64,

        uint8,
        uint16,
        uint32,
        uint64,

        real32,
        real64,

        string,
        bytes,

        interface,

        iid,    //interface id          = cid+iside
        ilid,   //interface local id    = clid+iside

        exception,
    };

    struct SPrimitive
    {
        PrimitiveKind   kind{PrimitiveKind::none};

        Sign            sign4Layout;
    };
}
