// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <type_traits>
#include <bit>

namespace dci::utils::endian
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::endian from, std::endian to, class T>
    T convert(T v) requires std::is_scalar_v<T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T> T l2b(T v) requires std::is_scalar_v<T>;
    template <class T> T b2l(T v) requires std::is_scalar_v<T>;
    template <class T> T n2b(T v) requires std::is_scalar_v<T>;
    template <class T> T b2n(T v) requires std::is_scalar_v<T>;
    template <class T> T l2n(T v) requires std::is_scalar_v<T>;
    template <class T> T n2l(T v) requires std::is_scalar_v<T>;
}

#include "endian.ipp"
