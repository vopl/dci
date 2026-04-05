/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "enumerateObjectFields.hpp"
#include <dci/stiac/serialization.hpp>

namespace dci::aup::impl::catalog
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline aup::catalog::ObjectPtr deserializeObject(auto& src)
    {
        try
        {
            aup::catalog::Object::Type otype;
            src >> otype;

            aup::catalog::ObjectPtr object;

            switch(otype)
            {
            case aup::catalog::Object::Type::file:
                object = std::make_unique<aup::catalog::File>();
                break;
            case aup::catalog::Object::Type::unit:
                object = std::make_unique<aup::catalog::Unit>();
                break;
            case aup::catalog::Object::Type::release:
                object = std::make_unique<aup::catalog::Release>();
                break;
            default:
                //throw aup::Exception{"unknown object type for deserialize catalog: "+std::to_string(static_cast<std::underlying_type_t<aup::catalog::Object::Type>>(otype))};
                return aup::catalog::ObjectPtr{};
            }

            catalog::enumerateObjectFields(object.get(), [&](auto& fld)
            {
                src >> fld;
            });

            return object;
        }
        catch(...) {}

        return aup::catalog::ObjectPtr{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline aup::catalog::ObjectPtr deserializeObject(Bytes&& src)
    {
        stiac::serialization::Arch arch{src.begin()};
        aup::catalog::ObjectPtr object = deserializeObject(arch);

        if(!arch.atEnd())
        {
            return aup::catalog::ObjectPtr{};
        }

        return object;
    }
}
