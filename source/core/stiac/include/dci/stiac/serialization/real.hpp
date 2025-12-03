// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "fixEndian.hpp"
#include <type_traits>

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept Real = std::is_floating_point_v<T> && (4 == sizeof(T) || 8 == sizeof(T));

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Real R>
    void save(auto& ar, const R& v)
    {
        R vFixed = fixEndian(v);
        ar.write(&vFixed, sizeof(vFixed));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Real R>
    void load(auto& ar, R& v)
    {
        R r;
        ar.read(&r, sizeof(v));
        v = fixEndian(r);
    }
}
