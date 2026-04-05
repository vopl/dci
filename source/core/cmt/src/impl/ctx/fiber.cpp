/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "fiber.hpp"
#include <dci/mm/stack.hpp>
#include "../scheduler.hpp"
#include <dci/logger.hpp>

namespace dci::cmt::impl::ctx
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Fiber* Fiber::alloc(Scheduler* scheduler)
    {
        if constexpr(Engine<Fiber>::_needStack)
        {
            mm::Stack stack;
            stack.initialize();
            if(!stack.initialized())
            {
                dbgWarn("internal error");
                return nullptr;
            }

            Fiber* fiber;
            if(stack.growsDown())
            {
                fiber = reinterpret_cast<Fiber*>(stack.end() - sizeof(Fiber));
            }
            else
            {
                fiber = reinterpret_cast<Fiber*>(stack.begin());
            }

            new(fiber) Fiber{scheduler};

            if(stack.growsDown())
            {
                char* end = reinterpret_cast<char *>(fiber);
                fiber->constructFiber(
                            true,
                            stack.begin(),
                            static_cast<std::size_t>(end - stack.begin()));
            }
            else
            {
                char* begin = reinterpret_cast<char *>(fiber) + sizeof(Fiber);
                fiber->constructFiber(
                            false,
                            begin,
                            static_cast<std::size_t>(stack.end() - begin));
            }

            fiber->assignStackIfNeed(std::move(stack));

            return fiber;
        }
        else
        {
            Fiber* fiber = new Fiber{scheduler};
            fiber->constructFiber(false, nullptr, 0);
            return fiber;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Fiber::Fiber(Scheduler* scheduler)
        : _scheduler(scheduler)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Fiber::free()
    {
        if constexpr(Engine<Fiber>::_needStack)
        {
            StackOrNone<Engine<Fiber>::_needStack> stackOrNone{std::move(*this)};
            (void)stackOrNone;
            this->~Fiber();
        }
        else
        {
            delete this;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Fiber::~Fiber()
    {
        dbgAssert(!emplaced());
        dbgAssert(!_task);
        destructFiber();
        //_stack = NULL;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Fiber::setTask(task::Body* task)
    {
        dbgAssert(!_task);
        _task = task;
        dbgAssert(_task);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Fiber::resetTask()
    {
        _task = nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    task::Body* Fiber::task()
    {
        return _task;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Fiber::contextProc()
    {
        _scheduler->fiberStarted();

        dbgAssert(this == _scheduler->currentFiber());

        for(;;)
        {
            dbgAssert(this == _scheduler->currentFiber());
            dbgAssert(_task);

            _task->callAndDestroy();
            dbgAssert(_task);
            _task = nullptr;

            dbgAssert(this == _scheduler->currentFiber());
            _scheduler->taskCompleted();
        }
    }
}
