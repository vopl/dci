// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/idl/contract/id.hpp>
#include <dci/eid.hpp>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept Tuid = std::is_same_v<T, idl::contract::Id> || std::is_same_v<T, Eid>;

    enum class TuidStoreKind : uint8
    {
        map = 1,
        mapped = 2,
    };

    using TuidMappedValue = uint32;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Tuid T>
    void save(auto& ar, const T& v)
    {
        auto [mapped, isNew] = ar.mapTuid(v);

        if(isNew)
        {
            ar << TuidStoreKind::map;
            //ar << static_cast<const std::array<uint8, 16>&>(v);
            ar.write(v.data(), static_cast<uint32>(v.size()));
        }
        else
        {
            ar << TuidStoreKind::mapped;
        }

        ar << TuidMappedValue(mapped);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Tuid T>
    void load(auto& ar, T& v)
    {
        TuidStoreKind tuidStoreKind;
        ar >> tuidStoreKind;

        switch(tuidStoreKind)
        {
        case TuidStoreKind::map:
            {
                //ar >> static_cast<std::array<uint8, 16>&>(v);
                ar.read(v.data(), static_cast<uint32>(v.size()));

                TuidMappedValue tuidMappedValue;
                ar >> tuidMappedValue;
                if(!ar.mapTuid(tuidMappedValue, v))
                {
                    ar.fail("malformed input stream (corrupted data)");
                }
            }
            return;

        case TuidStoreKind::mapped:
            {
                TuidMappedValue tuidMappedValue;
                ar >> tuidMappedValue;
                if(!ar.unmapTuid(tuidMappedValue, v))
                {
                    ar.fail("malformed input stream (corrupted data)");
                }
            }
            return;
        }

        ar.fail("malformed input stream (corrupted data)");
    }
}
