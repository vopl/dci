// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    void save(auto& ar, const Variant<Ts...>& v)
    {
        ar << v.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    void save(auto& ar, Variant<Ts...>&& v)
    {
        ar << std::move(v).std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    void load(auto& ar, Variant<Ts...>& v)
    {
        ar >> v.std();
    }
}
