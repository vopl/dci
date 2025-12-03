// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "details/waiterCaller.hpp"

namespace dci::cmt
{
    inline namespace operators
    {
        template <details::CExpr T> constexpr auto operator!(T   v) { return details::Not{v}; }
        template <details::CVSrc T> constexpr auto operator!(T&& v) { return !details::Val{&v}; }

        template <details::CExpr L, details::CExpr R> constexpr auto operator ||(L   l, R   r) { return details::Or{l, r}; }
        template <details::CVSrc L, details::CExpr R> constexpr auto operator ||(L&& l, R   r) { return details::Val{&l} ||               r ; }
        template <details::CExpr L, details::CVSrc R> constexpr auto operator ||(L   l, R&& r) { return l                || details::Val{&r}; }
        template <details::CVSrc L, details::CVSrc R> constexpr auto operator ||(L&& l, R&& r) { return details::Val{&l} || details::Val{&r}; }

        template <details::CExpr L, details::CExpr R> constexpr auto operator &&(L   l, R   r) { return details::And{l, r}; }
        template <details::CVSrc L, details::CExpr R> constexpr auto operator &&(L&& l, R   r) { return details::Val{&l} &&               r ; }
        template <details::CExpr L, details::CVSrc R> constexpr auto operator &&(L   l, R&& r) { return l                && details::Val{&r}; }
        template <details::CVSrc L, details::CVSrc R> constexpr auto operator &&(L&& l, R&& r) { return details::Val{&l} && details::Val{&r}; }
    }
}
