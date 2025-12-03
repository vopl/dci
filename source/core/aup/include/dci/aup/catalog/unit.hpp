// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "object.hpp"

namespace dci::aup::catalog
{
    struct Unit : Object
    {
        std::string         _name;
        Set<std::string>    _extraAllowed;

        Type type() const override {return Object::Type::unit;}
    };

    using UnitPtr = std::unique_ptr<Unit>;
}
