/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../common.hpp"

namespace dci::qml::qmeta::api
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Name name, auto read, auto write, class SignatureR=CallableSignature<decltype(read)>, class SignatureW=CallableSignature<decltype(write)>>
    requires
    (
        SignatureR::_valid && //Type(const Gadget&)
        SignatureW::_valid && //void(Gadget&, const Type&)

        SignatureR::ParamList::_size == 1 &&
        SignatureW::ParamList::_size == 2 &&

        !std::is_same_v<void, typename SignatureR::Return> &&
        std::is_same_v<void, typename SignatureW::Return> &&

        std::is_invocable_r_v<
            std::decay_t<typename SignatureW::ParamList::template Get<1>>,
            decltype(read),
            const std::decay_t<typename SignatureW::ParamList::template Get<0>>&
        >&&
        std::is_invocable_r_v<
            void,
            decltype(write),
            std::decay_t<typename SignatureR::ParamList::template Get<0>>&,
            const std::decay_t<typename SignatureR::Return>&
        >
    )
    struct PropRW
    {
        static constexpr bool _isProp {true};
        static constexpr bool _isPropRW {true};
        static constexpr bool _isPropR {true};
        static constexpr bool _isPropW {true};
        static constexpr auto _name {name};
        static constexpr auto _read {read};
        static constexpr auto _write {write};

        using Type      = typename SignatureR::Return;
        using Strings   = VList<_name>;
        using MetaTypes = TList<Type>;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Name name, auto read, class SignatureR=CallableSignature<decltype(read)>>
    requires
    (
        SignatureR::_valid && //Type(const Gadget&)
        SignatureR::ParamList::_size == 1 &&
        !std::is_same_v<void, typename SignatureR::Return>
    )
    struct PropR
    {
        static constexpr bool _isProp {true};
        static constexpr bool _isPropRW {false};
        static constexpr bool _isPropR {true};
        static constexpr bool _isPropW {false};
        static constexpr auto _name {name};
        static constexpr auto _read {read};

        using Type      = typename SignatureR::Return;
        using Strings   = VList<_name>;
        using MetaTypes = TList<Type>;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Name name, auto write, class SignatureW=CallableSignature<decltype(write)>>
    requires
    (
        SignatureW::_valid && //void(Gadget&, const Type&)
        SignatureW::ParamList::_size == 2 &&
        std::is_same_v<void, typename SignatureW::Return>
    )
    struct PropW
    {
        static constexpr bool _isProp {true};
        static constexpr bool _isPropRW {false};
        static constexpr bool _isPropR {false};
        static constexpr bool _isPropW {true};
        static constexpr auto _name {name};
        static constexpr auto _write {write};

        using Type      = std::decay_t<typename SignatureW::ParamList::template Get<1>>;
        using Strings   = VList<_name>;
        using MetaTypes = TList<Type>;
    };

    template <class T, class=void> struct IsPropImpl: Value<false>{};
    template <class T> struct IsPropImpl<T, std::void_t<decltype(T::_isProp)>>: Value<T::_isProp>{};
    template <class T> using IsProp = Value<IsPropImpl<T>::_v>;

    template <class T, class=void> struct IsPropRWImpl: Value<false>{};
    template <class T> struct IsPropRWImpl<T, std::void_t<decltype(T::_isPropRW)>>: Value<T::_isPropRW>{};
    template <class T> using IsPropRW = Value<IsPropRWImpl<T>::_v>;

    template <class T, class=void> struct IsPropRImpl: Value<false>{};
    template <class T> struct IsPropRImpl<T, std::void_t<decltype(T::_isPropR)>>: Value<T::_isPropR>{};
    template <class T> using IsPropR = Value<IsPropRImpl<T>::_v>;

    template <class T, class=void> struct IsPropWImpl: Value<false>{};
    template <class T> struct IsPropWImpl<T, std::void_t<decltype(T::_isPropW)>>: Value<T::_isPropW>{};
    template <class T> using IsPropW = Value<IsPropWImpl<T>::_v>;
}
