// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/idl/contract/lid.hpp>
#include <dci/idl/contract/id.hpp>
#include <dci/idl/contract/lidRegistry.hpp>
#include <dci/logger.hpp>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void save(auto& ar, const idl::contract::Lid& v)
    {
        ar << idl::contract::lidRegistry.get(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void load(auto& ar, idl::contract::Lid& v)
    {
        idl::contract::Id id;
        ar >> id;
        v = idl::contract::lidRegistry.get(id);

        if(!v)
        {
            //ar.fail((std::string{"unknown contract id provided: "} + id.toHex()).data());
            LOGI("unknown contract id provided: " << id.toHex() << ", memorized");
            v = idl::contract::lidRegistry.emplace(id);
        }
    }
}
