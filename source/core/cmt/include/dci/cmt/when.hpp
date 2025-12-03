// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "details/waiterCaller.hpp"
#include "future.hpp"
#include <cstdint>

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <details::CWaitableOrContainer... Waitables> Future<std::size_t> whenAny(Waitables&...);
    template <details::CWaitableOrContainer... Waitables> Future<void>        whenAll(Waitables&...);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <details::CExpr Expr> std::bitset<details::expr::countWaitables<Expr>> when(Expr&& expr);
    template <details::CVSrc VSrc> std::bitset<1                                  > when(VSrc&& vSrc);
}

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <details::CWaitableOrContainer... Waitables> Future<std::size_t> whenAny(Waitables&... waitables)
    {
        return details::waiterCaller<details::Kind::any, false>(waitables...);
    }

    template <details::CWaitableOrContainer... Waitables> Future<void> whenAll(Waitables&... waitables)
    {
        return details::waiterCaller<details::Kind::all, false>(waitables...);
    }

    template <details::CExpr Expr> std::bitset<details::expr::countWaitables<Expr>()> when(Expr&& expr)
    {
        return details::waiterCaller<details::Kind::expr, false>(expr);
    }

    template <details::CVSrc VSrc> std::bitset<1> when(VSrc&& vSrc)
    {
        return when(details::Val{&vSrc});
    }
}
