// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/utils/bits.hpp>
#include "../smallIntegral.hpp"
#include "fixEndian.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> concept SmallIntegralSigned = T::_isSmallIntegral && T::_isSigned;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <SmallIntegralSigned T>
    void save(auto& ar, const T& v)
    {
        static_assert(sizeof(v)<=8);

        if constexpr(1 == sizeof(v))
        {
            ar.write(&v._v, 1);
            return;
        }

        if constexpr(2 == sizeof(v))
        {
            typename T::V vFixed = fixEndian(v._v);
            ar.write(&vFixed, 2);
            return;
        }

        if constexpr(4 <= sizeof(v))
        {
            using U = std::make_unsigned_t<typename T::V>;

            U vZigZag = static_cast<U>((v._v << 1) ^ (v._v >> (utils::bits::bitsof(v._v)-1)));
            ar << SmallIntegral<U>{vZigZag};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <SmallIntegralSigned T>
    void load(auto& ar, T& v)
    {
        static_assert(sizeof(v)<=8);

        if constexpr(1 == sizeof(v))
        {
            ar.read(&v, 1);
            return;
        }

        if constexpr(2 == sizeof(v))
        {
            typename T::V s;
            ar.read(&s, 2);
            v._v = fixEndian(s);
            return;
        }

        if constexpr(4 <= sizeof(v))
        {
            using U = std::make_unsigned_t<typename T::V>;
            SmallIntegral<U> vZigZag;
            ar >> vZigZag;

            v._v = static_cast<typename T::V>((vZigZag._v >> 1)) ^ -static_cast<typename T::V>(vZigZag._v & 1);
        }
    }
}
