/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/utils/intrusiveDlist.hpp>
#include <dci/cmt/task/key.hpp>
#include <dci/cmt/task/state.hpp>
#include <dci/sbs/owner.hpp>

namespace dci::cmt::task
{
    class Body;
    using CallAndDestroyExecutor = void (*)(Body* task, bool call, bool destroy) noexcept(true);
}

namespace dci::cmt::impl::ctx
{
    class Fiber;
}

namespace dci::cmt::impl
{
    class Scheduler;
}

namespace dci::cmt::impl::task
{
    class Owner;
    class Face;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Body final
        : public utils::IntrusiveDlistElement<Body, Scheduler>
        , public utils::IntrusiveDlistElement<Body, Owner>

    {
    public:
        Body(Owner* owner, cmt::task::CallAndDestroyExecutor callAndDestroyExecutor);
        ~Body();

    public:
        void setFiber(ctx::Fiber* fiber);
        ctx::Fiber* fiber();
        void callAndDestroy() noexcept(true);
        void destroy() noexcept(true);

    public:
        void subscribe(Face* face);
        void unsubscribe(Face* face);

    public:
        void setState(cmt::task::State state);

    public:
        cmt::task::Key key() const;
        bool isCurrent() const;
        cmt::task::State state() const;

        void stop(bool throwSelf = true);
        bool stopRequested() const;

        void ownTo(Owner* owner);
        Owner* owner();
        void detachOwner();

        sbs::Owner* sbsOwner();
    private:
        ctx::Fiber* _fiber {nullptr};

        sbs::Owner _sbsOwner;

        Owner*  _owner;
        cmt::task::CallAndDestroyExecutor _callAndDestroyExecutor;

    private:
        cmt::task::State    _state {cmt::task::State::null};
        bool                _stopRequested {false};

    private:
        utils::IntrusiveDlist<Face, Body> _faces;
    };

}
