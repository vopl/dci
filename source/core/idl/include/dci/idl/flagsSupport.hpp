/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <type_traits>
#include "introspection.hpp"

namespace dci::idl::flagsSupport
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        template <class T> concept Flags    = introspection::isFlags<T>;
        template <class T> concept Integral = std::is_integral_v<T>;

        template <Flags F> std::underlying_type_t<F>  uv(F                          f) {return  static_cast<std::underlying_type_t<F> >(                    f );}
        template <Flags F> std::underlying_type_t<F>& ur(F&                         f) {return *static_cast<std::underlying_type_t<F>*>(static_cast<void*>(&f));}

        template <Flags F, Integral I> F              fv(I                          i) {return  static_cast<F                         >(                    i );}
        template <Flags F> F&                         fr(std::underlying_type_t<F>& u) {return *static_cast<F*                        >(static_cast<void*>(&u));}
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Flags    L, Integral R> constexpr auto operator==(L l, R r) {return uv(l) ==    r ;}
    template <Integral L, Flags    R> constexpr auto operator==(L l, R r) {return l     == uv(r);}
    template <Flags    L, Flags    R> constexpr auto operator==(L l, R r) {return uv(l) == uv(r);}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Flags    L, Integral R> constexpr auto operator<=>(L l, R r) {return uv(l) <=>    r ;}
    template <Integral L, Flags    R> constexpr auto operator<=>(L l, R r) {return l     <=> uv(r);}
    template <Flags    L, Flags    R> constexpr auto operator<=>(L l, R r) {return uv(l) <=> uv(r);}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Flags F>                constexpr bool operator!(F f) {return !uv(f) ;}

    template <Flags    L, Integral R> constexpr bool operator&&(L l, R r) {return uv(l) &&    r ;}
    template <Integral L, Flags    R> constexpr bool operator&&(L l, R r) {return l     && uv(r);}
    template <Flags    L, Flags    R> constexpr bool operator&&(L l, R r) {return uv(l) && uv(r);}

    template <Flags    L, Integral R> constexpr bool operator||(L l, R r) {return uv(l) ||    r ;}
    template <Integral L, Flags    R> constexpr bool operator||(L l, R r) {return l     || uv(r);}
    template <Flags    L, Flags    R> constexpr bool operator||(L l, R r) {return uv(l) || uv(r);}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Flags F>                constexpr F operator~(F f) {return fv<F>(~uv(f));}

    template <Flags    L, Integral R> constexpr L operator &(L l, R r) {return fv<L>(uv(l) &     r );}
    template <Integral L, Flags    R> constexpr L operator &(L l, R r) {return          l  &  uv(r) ;}
    template <Flags    L, Flags    R> constexpr L operator &(L l, R r) {return fv<L>(uv(l) &  uv(r));}

    template <Flags    L, Integral R> constexpr L operator |(L l, R r) {return fv<L>(uv(l) |     r );}
    template <Integral L, Flags    R> constexpr L operator |(L l, R r) {return          l  |  uv(r) ;}
    template <Flags    L, Flags    R> constexpr L operator |(L l, R r) {return fv<L>(uv(l) |  uv(r));}

    template <Flags    L, Integral R> constexpr L operator ^(L l, R r) {return fv<L>(uv(l) ^     r );}
    template <Integral L, Flags    R> constexpr L operator ^(L l, R r) {return          l  ^  uv(r) ;}
    template <Flags    L, Flags    R> constexpr L operator ^(L l, R r) {return fv<L>(uv(l) ^  uv(r));}

    template <Flags    L, Integral R> constexpr L operator>>(L l, R r) {return fv<L>(uv(l) >>    r );}
    template <Integral L, Flags    R> constexpr L operator>>(L l, R r) {return          l  >> uv(r) ;}
    template <Flags    L, Flags    R> constexpr L operator>>(L l, R r) {return fv<L>(uv(l) >> uv(r));}

    template <Flags    L, Integral R> constexpr L operator<<(L l, R r) {return fv<L>(uv(l) <<    r );}
    template <Integral L, Flags    R> constexpr L operator<<(L l, R r) {return          l  << uv(r) ;}
    template <Flags    L, Flags    R> constexpr L operator<<(L l, R r) {return fv<L>(uv(l) << uv(r));}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <Flags    L, Integral R> constexpr L& operator &=(L& l, R r) {return fr<L>(ur(l) &=     r );}
    template <Integral L, Flags    R> constexpr L& operator &=(L& l, R r) {return          l  &=  uv(r) ;}
    template <Flags    L, Flags    R> constexpr L& operator &=(L& l, R r) {return fr<L>(ur(l) &=  uv(r));}

    template <Flags    L, Integral R> constexpr L& operator |=(L& l, R r) {return fr<L>(ur(l) |=     r );}
    template <Integral L, Flags    R> constexpr L& operator |=(L& l, R r) {return          l  |=  uv(r) ;}
    template <Flags    L, Flags    R> constexpr L& operator |=(L& l, R r) {return fr<L>(ur(l) |=  uv(r));}

    template <Flags    L, Integral R> constexpr L& operator ^=(L& l, R r) {return fr<L>(ur(l) ^=     r );}
    template <Integral L, Flags    R> constexpr L& operator ^=(L& l, R r) {return          l  ^=  uv(r) ;}
    template <Flags    L, Flags    R> constexpr L& operator ^=(L& l, R r) {return fr<L>(ur(l) ^=  uv(r));}

    template <Flags    L, Integral R> constexpr L& operator>>=(L& l, R r) {return fr<L>(ur(l) >>=    r );}
    template <Integral L, Flags    R> constexpr L& operator>>=(L& l, R r) {return          l  >>= uv(r) ;}
    template <Flags    L, Flags    R> constexpr L& operator>>=(L& l, R r) {return fr<L>(ur(l) >>= uv(r));}

    template <Flags    L, Integral R> constexpr L& operator<<=(L& l, R r) {return fr<L>(ur(l) <<=    r );}
    template <Integral L, Flags    R> constexpr L& operator<<=(L& l, R r) {return          l  <<= uv(r) ;}
    template <Flags    L, Flags    R> constexpr L& operator<<=(L& l, R r) {return fr<L>(ur(l) <<= uv(r));}
}
