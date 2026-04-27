/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "waitable.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waitable::Waitable(bool (* canAcquire)(const Waitable*), bool (* tryAcquire)(Waitable*))
        : _links()
        , _canAcquire(canAcquire)
        , _tryAcquire(tryAcquire)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waitable::~Waitable()
    {
        _links.each([this](WWLink* link)
        {
            dbgAssert(this == link->_waitable);
            link->_waiter->waitableDead(link);
        });

        dbgAssert(_links.empty());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::tryDestruction(Waitable *)
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::wait()
    {
        if(tryAcquire())
        {
            return;
        }

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waitable::canAcquire() const
    {
        return _canAcquire(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waitable::tryAcquire()
    {
        return _tryAcquire(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::beginAcquire(WWLink* link)
    {
        dbgAssert(!Scheduler::instance().currentFiber() || !Scheduler::instance().currentFiber()->task()->stopRequested());

        dbgAssert(this == link->_waitable);
        dbgAssert(!_links.contains(link));

        _links.pushBack(link);
        _linksAmount++;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::endAcquire(WWLink* link)
    {
        dbgAssert(this == link->_waitable);
        dbgAssert(_links.contains(link));

        _links.remove(link);
        _linksAmount--;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::throwTaskStopIfNeed()
    {
        ctx::Fiber* currentFiber = Scheduler::instance().currentFiber();

        if(currentFiber)
        {
            dbgAssert(currentFiber->task());
            if(currentFiber->task()->stopRequested())
            {
                throw cmt::task::Stop{};
            }
        }
    }

}
