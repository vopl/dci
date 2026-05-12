/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
    template <details::CExpr Expr> Future<std::bitset<details::expr::countWaitables<Expr>>> when(Expr&& expr);
    template <details::CVSrc VSrc> Future<std::bitset<1                                  >> when(VSrc&& vSrc);
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

    template <details::CExpr Expr> Future<std::bitset<details::expr::countWaitables<Expr>()>> when(Expr&& expr)
    {
        return details::waiterCaller<details::Kind::expr, false>(expr);
    }

    template <details::CVSrc VSrc> Future<std::bitset<1>> when(VSrc&& vSrc)
    {
        return when(details::Val{&vSrc});
    }
}
