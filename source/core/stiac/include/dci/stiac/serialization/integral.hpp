// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include "fixEndian.hpp"
#include <type_traits>

namespace dci::stiac::serialization
{
    template <class T> concept Integral = std::is_integral_v<T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Integral I>
    void save(auto& ar, const I& v)
    {
        static_assert(sizeof(v)<=8);

        I vFixed = fixEndian(v);
        ar.write(&vFixed, sizeof(vFixed));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Integral I>
    void load(auto& ar, I& v)
    {
        static_assert(sizeof(v)<=8);

        I vUnfixed;
        ar.read(&vUnfixed, sizeof(vUnfixed));
        v = fixEndian(vUnfixed);
    }
}
