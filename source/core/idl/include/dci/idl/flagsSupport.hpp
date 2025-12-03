// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
