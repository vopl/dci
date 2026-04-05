/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/primitives.hpp>
#include <type_traits>
#include <functional>
#include <dci/utils/ct.hpp>
#include <dci/utils/tname.hpp>

namespace dci::idl::introspection
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct None{};

    enum class Kind
    {
        unknown,
        flags,
        enum_,
        struct_,
        exception,
        interface,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //common
    template <class T> constexpr auto typeName = utils::tname<T>;

    template <class T> constexpr Kind kind = Kind::unknown;

    template <class T> constexpr bool isKnown       = static_cast<int>(Kind::unknown)   != static_cast<int>(kind<T>);
    template <class T> constexpr bool isFlags       = static_cast<int>(Kind::flags)     == static_cast<int>(kind<T>);
    template <class T> constexpr bool isEnum        = static_cast<int>(Kind::enum_)     == static_cast<int>(kind<T>);
    template <class T> constexpr bool isStruct      = static_cast<int>(Kind::struct_)   == static_cast<int>(kind<T>);
    template <class T> constexpr bool isException   = static_cast<int>(Kind::exception) == static_cast<int>(kind<T>);
    template <class T> constexpr bool isInterface   = static_cast<int>(Kind::interface) == static_cast<int>(kind<T>);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //bases
    template <class T>              constexpr None  basesCount;
    template <class T, uint32 BI>   constexpr None  baseName;
    template <class T, uint32 BI>   struct          BaseType{using Result = None;};
    template <class T, uint32 BI>   using           baseType = typename BaseType<T, BI>::result;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //fields
    template <class T>              constexpr None  fieldsCount;
    template <class T, uint32 FI>   constexpr None  fieldName;
    template <class T, uint32 FI>   struct          FieldType{using Result = None;};
    template <class T, uint32 FI>   using           fieldType = typename FieldType<T, FI>::result;
    template <class T, uint32 FI>   constexpr None  fieldValue;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    //methods
    template <class T>                          constexpr None  methodsCount;
    template <class T, uint32 MI>               constexpr None  methodActivator;
    template <class T, uint32 MI>               constexpr None  methodName;
    template <class T, uint32 MI>               constexpr None  methodIn;
    template <class T, uint32 MI>               constexpr None  methodNoReply;

    template <class T, uint32 MI>               struct          MethodReplyType{using result = None;};
    template <class T, uint32 MI>               using           methodReplyType = typename MethodReplyType<T, MI>::result;

    template <class T, uint32 MI>               constexpr None  methodParamsCount;
    template <class T, uint32 MI, uint32 PI>    constexpr None  methodParamName;
    template <class T, uint32 MI, uint32 PI>    struct          MethodParamType{using result = None;};
    template <class T, uint32 MI, uint32 PI>    using           methodParamType = typename MethodParamType<T, MI, PI>::result;
}

namespace dci::idl::introspection
{
    namespace details
    {
        template <class Src, class Dst>
        using copyReference = std::conditional_t
            <
                std::is_rvalue_reference_v<Src>,
                std::add_rvalue_reference_t<Dst>,
                std::conditional_t
                    <
                        std::is_lvalue_reference_v<Src>,
                        std::add_lvalue_reference_t<Dst>,
                        std::remove_reference_t<Dst>
                    >
            >;

        template <class Src, class Dst>
        using copyConst = std::conditional_t
            <
                std::is_const_v<std::remove_reference_t<Src>>,
                std::add_const_t<Dst>,
                std::remove_const_t<Dst>
            >;

        template <class Src, class Dst>
        using copyVolatile = std::conditional_t
            <
                std::is_volatile_v<std::remove_reference_t<Src>>,
                std::add_volatile_t<Dst>,
                std::remove_volatile_t<Dst>
            >;

        template <class Src, class Dst>
        using copyRCV = copyReference<Src, copyConst<Src, copyVolatile<Src, Dst>>>;
    }

    template <class T, auto memberPointer>
    inline constexpr auto memberValue = [](auto&& o) -> decltype(auto)
    {
        auto& m = o.*memberPointer;
        return static_cast<details::copyRCV<decltype(o), decltype(m)>>(m);
    };

    template <class T, uint32 BI>
    inline constexpr auto baseValue = [](auto&& o) -> decltype(auto)
    {
        using B = baseType<T, BI>;
        return static_cast<details::copyRCV<decltype(o), B>>(o);
    };

    namespace details
    {
        template <uint32... BI, uint32... FI>
        constexpr decltype(auto) applyImpl(auto&& f, auto&& s, utils::ct::TList<utils::ct::Value<BI>...>, utils::ct::TList<utils::ct::Value<FI>...>)
        {
            using S = std::decay_t<decltype(s)>;
            return std::invoke(
                        std::forward<decltype(f)>(f),
                        baseValue<S, BI>(std::forward<decltype(s)>(s))...,
                        fieldValue<S, FI>(std::forward<decltype(s)>(s))...);
        }
    }

    constexpr decltype(auto) apply(auto&& f, auto&& s)
    {
        using S = std::decay_t<decltype(s)>;
        return details::applyImpl(
                std::forward<decltype(f)>(f),
                std::forward<decltype(s)>(s),
                utils::ct::MakeSeq<basesCount<S>>{},
                utils::ct::MakeSeq<fieldsCount<S>>{});
    }

    constexpr decltype(auto) applyBases(auto&& f, auto&& s)
    {
        using S = std::decay_t<decltype(s)>;
        return details::applyImpl(
                std::forward<decltype(f)>(f),
                std::forward<decltype(s)>(s),
                utils::ct::MakeSeq<basesCount<S>>{},
                utils::ct::TList<>{});
    }

    constexpr decltype(auto) applyFields(auto&& f, auto&& s)
    {
        using S = std::decay_t<decltype(s)>;
        return details::applyImpl(
                std::forward<decltype(f)>(f),
                std::forward<decltype(s)>(s),
                utils::ct::TList<>{},
                utils::ct::MakeSeq<fieldsCount<S>>{});
    }

    constexpr auto linearize(auto&& s)
    {
        return apply(
            [](auto&&... e){return std::tie(e...);},
            std::forward<decltype(s)>(s));
    }
}
