/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "owner.hpp"
#include "../scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::Owner()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::~Owner()
    {
        stopImpl(false, true);

        _tasks.each([](Body* task)
        {
            task->detachOwner();
        });
        dbgAssert(_tasks.empty());

        dbgAssert(_waiting.empty());
        _waiting.each([](Body* task)
        {
            task->detachOwner();
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::subscribe(Body* task)
    {
        dbgAssert(!_tasks.contains(task));
        _tasks.pushBack(task);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::unsubscribe(Body* task)
    {
        if(_waitingActive)
        {
            dbgAssert((task->utils::IntrusiveDlistElement<Body, Owner>::emplaced()));
            task->utils::IntrusiveDlistElement<Body, Owner>::retire();
        }
        else
        {
            dbgAssert(_waiting.empty());
            dbgAssert(_tasks.contains(task));
            _tasks.remove(task);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Owner::stopRequested() const
    {
        return _stopRequested;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Owner::empty() const
    {
        return _tasks.empty() && _waiting.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::flush(bool andWait)
    {
        return stopImpl(true, andWait);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::stop(bool andWait)
    {
        return stopImpl(false, andWait);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::wait()
    {
        if(_waitingActive)
            return;
        _waitingActive = true;

        Scheduler& scheduler = Scheduler::instance();
        Body* currentTask = scheduler.currentTask();

        std::size_t prevCount{};
        for(;;)
        {
            std::size_t count{};
            while(!_tasks.empty())
            {
                Body* task = _tasks.popFront();
                _waiting.pushBack(task);
                ++count;

                if(currentTask != task)
                    scheduler.switchTo(task);
            }
            _tasks = std::move(_waiting);

            if(std::exchange(prevCount, count) == count)
                break;
        }

        dbgAssert(_waitingActive);
        _waitingActive = false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::stopImpl(bool once, bool andWait)
    {
        _stopRequested = true;
        _tasks.each([](Body* task)
        {
            task->stop(false);
        });

        if(andWait)
            wait();

        if(once)
            _stopRequested = false;
    }
}
