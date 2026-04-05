/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/cmt/functions.hpp>
#include "impl/scheduler.hpp"

namespace dci::cmt
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool yield()
    {
        impl::Scheduler& s = impl::Scheduler::instance();

        dbgAssert(s.currentTask());
        if(s.currentTask()->stopRequested())
        {
            throw cmt::task::Stop{};
        }

        return s.yield();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool executeReadyFibers()
    {
        return impl::Scheduler::instance().executeReadyFibers();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void enumerateFibers(FiberEnumerationCallback cb, void* data)
    {
        return impl::Scheduler::instance().enumerateFibers(cb, data);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void spawn(task::Body* task)
    {
        return impl::Scheduler::instance().spawnTask(himpl::face2Impl(task));
    }
}
