// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include "../smallIntegral.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class K, class V>
    void save(auto& ar, const Map<K, V>& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));

        for(const auto& [ek, ev] : v)
        {
            ar << ek;
            ar << ev;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class K, class V>
    void save(auto& ar, Map<K, V>&& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));

        while(!v.empty())
        {
            auto node = v.extract(v.begin());
            ar << std::move(node.key());
            ar << std::move(node.mapped());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class K, class V>
    void load(auto& ar, Map<K, V>& v)
    {
        uint32 size;
        ar >> smallIntegral(size);

        if(size * 2 > ar.size())
        {
            ar.fail("malformed input stream (low data)");
        }

        v.clear();
        K ek;
        V ev;
        while(size)
        {
            ar >> ek;
            ar >> ev;
            v.emplace_hint(v.end(), std::move(ek), std::move(ev));
            size--;
        }
    }
}
