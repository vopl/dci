/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/cmt/task/face.hpp>
#include <dci/cmt/task/owner.hpp>
#include "impl/task/face.hpp"
#include "impl/task/owner.hpp"
#include "impl/scheduler.hpp"

namespace dci::cmt::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face()
        : himpl::FaceLayout<Face, impl::task::Face>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(const Face& from)
        : himpl::FaceLayout<Face, impl::task::Face>(himpl::face2Impl(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::Face(Face&& from)
        : himpl::FaceLayout<Face, impl::task::Face>(himpl::face2Impl(std::move(from)))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face::~Face()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::operator=(const Face& from)
    {
        return impl() = himpl::face2Impl(from);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::operator=(Face&& from)
    {
        return impl() = himpl::face2Impl(std::move(from));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Key Face::key() const
    {
        return impl().key();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Face::isCurrent() const
    {
        return impl().isCurrent();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    State Face::state() const
    {
        return impl().state();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::stop(bool throwSelf)
    {
        return impl().stop(throwSelf);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Face::stopRequested() const
    {
        return impl().stopRequested();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Face::ownTo(Owner* owner)
    {
        return impl().ownTo(himpl::face2Impl(owner));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner* Face::owner()
    {
        return himpl::impl2Face<Owner>(impl().owner());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner* Face::sbsOwner()
    {
        return impl().sbsOwner();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Face current()
    {
        return himpl::impl2Face<Face>(impl::task::Face{impl::Scheduler::instance().currentTask()});
    }
}
