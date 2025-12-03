// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
