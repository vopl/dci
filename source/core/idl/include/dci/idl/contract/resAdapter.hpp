/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/sbs/wire/transfer/ret.hpp>
#include <dci/cmt/future.hpp>
#include "../interface/exception/methodNotConnected.hpp"

namespace dci::idl::contract
{
    struct ResAdapter
    {
        template <class R, dci::sbs::wire::Agg agg>
        R operator()(dci::sbs::wire::transfer::Ret<R, agg>& ret)
        {
            return ret.detach();
        }

        template <class T, dci::sbs::wire::Agg agg>
        cmt::Future<T> operator()(dci::sbs::wire::transfer::Ret<cmt::Future<T>, agg>& ret)
        {
            if(ret._assigned)
            {
                return std::move(ret._value);
            }

            return cmt::readyFuture<T>(std::make_exception_ptr(interface::exception::MethodNotConnected()));
        }

        template <class T>
        std::vector<cmt::Future<T>> operator()(dci::sbs::wire::transfer::Ret<cmt::Future<T>, dci::sbs::wire::Agg::all>& ret)
        {
            return std::move(ret._value);
        }
    };
}
