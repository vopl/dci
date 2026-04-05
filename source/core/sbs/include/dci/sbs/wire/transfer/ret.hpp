/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../agg.hpp"
#include "../../exception/wireNotConnected.hpp"
#include <vector>

namespace dci::sbs::wire::transfer
{

    template <class R, Agg agg>
    struct Ret;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    struct Ret<void, Agg::first>
    {
        void detach() {}
    };

    template <>
    struct Ret<void, Agg::last>
    {
        void detach() {}
    };

    template <>
    struct Ret<void, Agg::all>
    {
        void detach() {}
    };

    namespace
    {
        template <class E, class R>
        void chargeException(const R&)
        {
            throw E{};
        }

        template <class E, class Future, class = typename Future::Promise::Future>
        void chargeException(Future& future)
        {
            typename Future::Promise promise;
            promise.resolveException(std::make_exception_ptr(E{}));
            future = promise.future();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R>
    struct Ret<R, Agg::first>
    {
        R       _value;
        bool    _assigned = false;

        R detach()
        {
            if(!_assigned)
            {
                chargeException<exception::WireNotConnected>(_value);
            }

            return std::move(_value);
        }
    };

    template <class R>
    struct Ret<R, Agg::last>
    {
        R       _value;
        bool    _assigned = false;

        R detach()
        {
            if(!_assigned)
            {
                chargeException<exception::WireNotConnected>(_value);
            }

            return std::move(_value);
        }
    };

    template <class R>
    struct Ret<R, Agg::all>
    {
        std::vector<R>  _value;

        std::vector<R> detach()
        {
            return std::move(_value);
        }
    };
}
