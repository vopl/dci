// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../common.hpp"

namespace dci::qml::qmeta::api
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Name key, Name value>
    struct Info
    {
        static constexpr bool _isInfo {true};
        static constexpr auto _key {key};
        static constexpr auto _value {value};

        using Strings = VList<_key, _value>;
    };

    template <class T, class=void> struct IsInfoImpl: Value<false>{};
    template <class T> struct IsInfoImpl<T, std::void_t<decltype(T::_isInfo)>>: Value<T::_isInfo>{};
    template <class T> using IsInfo = Value<IsInfoImpl<T>::_v>;
}
