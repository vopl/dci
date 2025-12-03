// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include "../smallIntegral.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, const Set<T>& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));

        for(const T& e : v)
        {
            ar << e;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void save(auto& ar, Set<T>&& v)
    {
        ar << smallIntegral(static_cast<uint32>(v.size()));

        while(!v.empty())
        {
            auto node = v.extract(v.begin());
            ar << std::move(node.value());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void load(auto& ar, Set<T>& v)
    {
        uint32 size;
        ar >> smallIntegral(size);

        if(size * 1 > ar.size())
        {
            ar.fail("malformed input stream (low data)");
        }

        v.clear();

        T e;
        while(size)
        {
            ar >> e;
            v.emplace_hint(v.end(), std::move(e));
            size--;
        }
    }
}
