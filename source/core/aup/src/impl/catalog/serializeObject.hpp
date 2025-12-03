// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "enumerateObjectFields.hpp"
#include <dci/stiac/serialization.hpp>

namespace dci::aup::impl::catalog
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class O>
    inline void serializeObject(const std::unique_ptr<O>& objectPtr, auto& dst)
    {
        dst << objectPtr->type();
        catalog::enumerateObjectFields(objectPtr.get(), [&](const auto& fld)
        {
            dst << fld;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class O>
    inline Bytes serializeObject(const std::unique_ptr<O>& objectPtr)
    {
        Bytes blob;
        stiac::serialization::Arch arch{blob.begin()};
        serializeObject(objectPtr, arch);
        return blob;
    }
}
