/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/sbs/box.hpp>
#include "impl/box.hpp"

namespace dci::sbs
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Box::Box()
        : himpl::FaceLayout<Box, impl::Box>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Box::~Box()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Box::empty() const
    {
        return impl().empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::push(Subscription* subscription)
    {
        return impl().push(himpl::face2Impl(subscription));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::removeAndDelete(Subscription* subscription)
    {
        return impl().removeAndDelete(himpl::face2Impl(subscription));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::removeAndDeleteAll()
    {
        return impl().removeAndDeleteAll();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Box::activate(void* context, std::uint_fast8_t flags)
    {
        return impl().activate(context, flags);
    }
}
