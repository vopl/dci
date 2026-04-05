/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/cmt/details/waiter.hpp>
#include "impl/details/waiter.hpp"

namespace dci::cmt::details
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::Waiter(WWLink* links, std::size_t amount)
        : himpl::FaceLayout<Waiter, impl::details::Waiter>(links, amount)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::Waiter(WWLink* links, std::size_t amount, void(*cb)(void* cbData), void* cbData)
        : himpl::FaceLayout<Waiter, impl::details::Waiter>(links, amount, cb, cbData)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::~Waiter()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::any(std::size_t* acquiredIndex)
    {
        return impl().any(acquiredIndex);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::all()
    {
        return impl().all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::expr(ExprEvaluator ee, void* eeData, std::byte* bitsInBytes)
    {
        return impl().expr(ee, eeData, bitsInBytes);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::reset()
    {
        return impl().reset();
    }

}
