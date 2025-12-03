// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "details/waiterCaller.hpp"
#include <cstdint>

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <details::CWaitableOrContainer... Waitables> std::size_t waitAny (Waitables&... waitables);
    template <details::CWaitableOrContainer... Waitables> void        waitAll (Waitables&... waitables);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <details::CExpr Expr> std::bitset<details::expr::countWaitables<Expr>()> wait(Expr&& expr);
    template <details::CVSrc VSrc> std::bitset<1                                    > wait(VSrc&& vSrc);
}

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <details::CWaitableOrContainer... Waitables> std::size_t waitAny(Waitables&... waitables)
    {
        return details::waiterCaller<details::Kind::any>(waitables...);
    }

    template <details::CWaitableOrContainer... Waitables> void waitAll(Waitables&... waitables)
    {
        return details::waiterCaller<details::Kind::all>(waitables...);
    }

    template <details::CExpr Expr> std::bitset<details::expr::countWaitables<Expr>()> wait(Expr&& expr)
    {
        return details::waiterCaller<details::Kind::expr>(expr);
    }

    template <details::CVSrc VSrc> std::bitset<1> wait(VSrc&& vSrc)
    {
        return wait(details::Val{&vSrc});
    }
}
