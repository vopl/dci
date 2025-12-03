// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "object.hpp"
#include <dci/primitives.hpp>


namespace dci::aup::catalog
{
    struct File : Object
    {
        enum class Kind
        {
            null        = 0x00,

            runtime     = 0x01,
            resource    = 0x02,
            rdep        = 0x03,

            test        = 0x11,
            debug       = 0x12,

            bdep        = 0x21,
            include     = 0x22,
            idl         = 0x23,
            cmm         = 0x24,

            src         = 0x31,
        };

        Kind            _kind {};
        std::string     _path;
        uint16          _perms {};
        uint32          _size {};
        Oid             _content {};

        Object::Type type() const override {return Object::Type::file;}
    };

    using FilePtr = std::unique_ptr<File>;
}
