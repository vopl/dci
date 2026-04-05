/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
