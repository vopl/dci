/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "scheduler.hpp"
#include "ctx/fiber.hpp"
#include <dci/cmt/task/stop.hpp>
#include <dci/cmt/task/face.hpp>
#include "task/face.hpp"

#include <algorithm>
#include <utility>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scheduler::Scheduler()
        : _currentFiber(nullptr)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scheduler::~Scheduler()
    {
        {
            dbgAssert(_spawnedTasks.empty());

            _spawnedTasks.flush([](task::Body* task)
            {
                task->destroy();
            });
        }

        auto flushFibers = [](utils::IntrusiveDlist<ctx::Fiber, Scheduler>& fibers)
        {
            fibers.flush([](ctx::Fiber* fiber)
            {
                task::Body* task = fiber->task();
                if(task)
                {
                    //task->destroy(); секции с пользовательским кодом могут быть уже выгружены на этот момент
                    fiber->resetTask();
                }

                dbgAssert(!fiber->task());
                fiber->free();
            });
        };

        dbgAssert(_hold.empty());
        flushFibers(_hold);

        //dbgAssert(_empty.empty());
        flushFibers(_empty);

        dbgAssert(_ready4Stop.empty());
        flushFibers(_ready4Stop);

        dbgAssert(_ready.empty());
        flushFibers(_ready);

        dbgAssert(_readyLowPriority.empty());
        flushFibers(_readyLowPriority);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scheduler& Scheduler::instance()
    {
        return _instance;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::spawnTask(task::Body* task)
    {
        dbgAssert(task);
        dbgAssert(cmt::task::State::null == task->state());

        _spawnedTasks.push(task);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Scheduler::yield(std::uint32_t rfk)
    {
        ctx::Fiber* nextFiber = dequeueReadyFiber(rfk);
        if(!nextFiber)
            return false;

        dbgAssert(nextFiber != _currentFiber);
        dbgAssert(nextFiber->task());
        nextFiber->task()->setState(cmt::task::State::work);

        dbgAssert(_currentFiber);
        dbgAssert(!_currentFiber->emplaced());

        task::Body* currentTask = _currentFiber->task();
        dbgAssert(currentTask);
        dbgAssert(cmt::task::State::work == currentTask->state());

        ctx::Fiber* currentFiber = _currentFiber;

        if(currentTask->stopRequested())
        {
            currentTask->setState(cmt::task::State::ready4Stop);
            _ready4Stop.push(currentFiber);
        }
        else
        {
            currentTask->setState(cmt::task::State::readyLowPriority);
            _readyLowPriority.push(currentFiber);
        }

        f2f(currentFiber, nextFiber);

        dbgAssert(!currentFiber->emplaced());
        dbgAssert(currentFiber == _currentFiber);
        dbgAssert(currentFiber->task() == currentTask);
        dbgAssert(cmt::task::State::work == currentTask->state());

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::hold()
    {
        dbgAssert(_currentFiber);

        task::Body* task = _currentFiber->task();
        (void)task;

        dbgAssert(task);
        dbgAssert(!task->stopRequested());
        dbgAssert(cmt::task::State::work == task->state());

        ctx::Fiber* current = _currentFiber;
        dbgAssert(!current->emplaced());
        task->setState(cmt::task::State::hold);
        _hold.push(current);

        if(ctx::Fiber* next = dequeueReadyFiber())
        {
            dbgAssert(next->task());
            next->task()->setState(cmt::task::State::work);
            f2f(current, next);
        }
        else
        {
            f2r(current);
        }

        dbgAssert(current == _currentFiber);
        dbgAssert(!current->emplaced());
        dbgAssert(current->task() == task);
        dbgAssert(cmt::task::State::work == task->state());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::ready(ctx::Fiber* fiber)
    {
        dbgAssert(fiber != _currentFiber);

        task::Body* task = fiber->task();
        dbgAssert(task);

        if(cmt::task::State::hold == task->state())
        {
            dbgAssert(fiber->emplaced());
            dbgAssert(_hold.contains(fiber));
            _hold.remove(fiber);

            if(task->stopRequested())
            {
                task->setState(cmt::task::State::ready4Stop);
                _ready4Stop.push(fiber);
            }
            else
            {
                task->setState(cmt::task::State::ready);
                _ready.push(fiber);
            }
        }
        else
        {
            dbgAssert(cmt::task::State::ready == task->state() || cmt::task::State::readyLowPriority == task->state() || cmt::task::State::ready4Stop == task->state());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Scheduler::executeReadyFibers()
    {
        dbgAssert(!_currentFiber);
        if(_currentFiber)
        {
            dbgWarn("executeReadyFibers available only from root context");
            std::abort();
        }

        bool res = false;

        while(ctx::Fiber* next = dequeueReadyFiber())
        {
            res = true;
            dbgAssert(!next->emplaced());
            task::Body* task = next->task();
            dbgAssert(task);
            dbgAssert(cmt::task::State::ready == task->state() || cmt::task::State::readyLowPriority == task->state() || cmt::task::State::ready4Stop == task->state());
            task->setState(cmt::task::State::work);
            r2f(next);

            dbgAssert(!_currentFiber);
        }

        dbgAssert(_ready4Stop.empty());
        dbgAssert(_ready.empty());
        dbgAssert(_spawnedTasks.empty());
        dbgAssert(_readyLowPriority.empty());

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Scheduler::switchTo(task::Body* nextTask)
    {
        dbgAssert(nextTask);

        ctx::Fiber* nextFiber = nextTask->fiber();
        switch(nextTask->state())
        {
        case cmt::task::State::null:
        case cmt::task::State::work:
        case cmt::task::State::hold:
            return false;
        case cmt::task::State::ready4Stop:
            dbgAssert(_ready4Stop.contains(nextFiber));
            _ready4Stop.remove(nextFiber);
            break;
        case cmt::task::State::ready:
            dbgAssert(_ready.contains(nextFiber));
            _ready.remove(nextFiber);
            break;
        case cmt::task::State::readyLowPriority:
            dbgAssert(_readyLowPriority.contains(nextFiber));
            _readyLowPriority.remove(nextFiber);
            break;
        }
        nextTask->setState(cmt::task::State::work);

        ctx::Fiber* currentFiber = _currentFiber;

        if(currentFiber)
        {
            dbgAssert(!currentFiber->emplaced());

            task::Body* currentTask = currentFiber->task();
            if(currentTask->stopRequested())
            {
                currentTask->setState(cmt::task::State::ready4Stop);
                _ready4Stop.push(currentFiber);
            }
            else
            {
                currentTask->setState(cmt::task::State::readyLowPriority);
                _readyLowPriority.push(currentFiber);
            }

            f2f(currentFiber, nextFiber);

            dbgAssert(currentFiber == _currentFiber);
            dbgAssert(!currentFiber->emplaced());
            dbgAssert(currentFiber->task() == currentTask);
            dbgAssert(cmt::task::State::work == currentTask->state());
        }
        else
        {
            r2f(nextFiber);
            dbgAssert(!_currentFiber);
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::enumerateFibers(FiberEnumerationCallback cb, void* data)
    {
        dbgAssert(!_fiberEnumerationCallback);
        _fiberEnumerationCallback = cb;
        _fiberEnumerationCallbackData = data;

        _enumerateInitiator = std::exchange(_currentFiber, {});
        if(_enumerateInitiator)
        {
            //enumerate initiator
            _fiberEnumerationCallback(_enumerateInitiator->task()->state(), _fiberEnumerationCallbackData);

            //switch to root
            _enumerateInitiator->switchTo(&_rootContext, false);
        }
        else
        {
            //already in root, enumerate
            _fiberEnumerationCallback(cmt::task::State::null, _fiberEnumerationCallbackData);
        }

        auto traverseList = [&](utils::IntrusiveDlist<ctx::Fiber, Scheduler>& list)
        {
            dbgAssert(!_enumerateFiber);
            _enumerateFiber = list.first();
            if(_enumerateFiber)
            {
                if(_enumerateInitiator)
                    _enumerateInitiator->switchTo(_enumerateFiber, false);
                else
                    _rootContext.switchTo(_enumerateFiber);
                dbgAssert(!_enumerateFiber);
            }
        };

        traverseList(_ready4Stop);
        traverseList(_ready);
        traverseList(_readyLowPriority);
        traverseList(_hold);

        //already switched back to original fiber or root
        _currentFiber = std::exchange(_enumerateInitiator, {});

        dbgAssert(cb == _fiberEnumerationCallback);
        _fiberEnumerationCallback = {};
        _fiberEnumerationCallbackData = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ctx::Fiber* Scheduler::currentFiber()
    {
        return _currentFiber;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    task::Body* Scheduler::currentTask()
    {
        if(!_currentFiber)
        {
            return nullptr;
        }

        return _currentFiber->task();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::fiberStarted()
    {
        handleFiberAwake();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::taskCompleted()
    {
        dbgAssert(_currentFiber);
        ctx::Fiber* current = _currentFiber;
        dbgAssert(!current->task());
        _empty.push(current);

        if(ctx::Fiber* nextFiber = dequeueReadyFiber())
        {
            dbgAssert(!nextFiber->emplaced());
            dbgAssert(nextFiber->task());
            task::Body* nextTask = nextFiber->task();
            dbgAssert(cmt::task::State::ready == nextTask->state() || cmt::task::State::readyLowPriority == nextTask->state() || cmt::task::State::ready4Stop == nextTask->state());
            nextTask->setState(cmt::task::State::work);
            if(current != nextFiber)
            {
                f2f(current, nextFiber);
            }
        }
        else
        {
            f2r(current);
        }

        dbgAssert(current == _currentFiber);
        dbgAssert(!current->emplaced());
        dbgAssert(current->task());
        dbgAssert(cmt::task::State::work == current->task()->state());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ctx::Fiber* Scheduler::dequeueReadyFiber(std::uint32_t rfk)
    {
        if(rfk & rfk_4Stop)
        {
            ctx::Fiber* fiber = _ready4Stop.shift();
            if(fiber)
            {
                dbgAssert(fiber->task());
                dbgAssert(cmt::task::State::ready4Stop == fiber->task()->state());
                dbgAssert(fiber == fiber->task()->fiber());
                return fiber;
            }
        }

        if(rfk & rfk_regular)
        {
            ctx::Fiber* fiber = _ready.shift();
            if(fiber)
            {
                dbgAssert(fiber->task());
                dbgAssert(cmt::task::State::ready == fiber->task()->state());
                dbgAssert(fiber == fiber->task()->fiber());
                return fiber;
            }
        }

        if(rfk & rfk_new4SpawnedTasks)
        {
            task::Body* task = _spawnedTasks.shift();
            if(task)
            {
                ctx::Fiber* fiber = _empty.shift();

                if(!fiber)
                    fiber =  ctx::Fiber::alloc(this);

                if(!fiber)
                {
                    dbgWarn("unable to allocate new fiber");
                    std::abort();
                }

                dbgAssert(!fiber->task());
                fiber->setTask(task);
                task->setFiber(fiber);
                task->setState(cmt::task::State::ready);
                return fiber;
            }
        }

        if(rfk & rfk_lowPriority)
        {
            ctx::Fiber* fiber = _readyLowPriority.shift();
            if(fiber)
            {
                dbgAssert(fiber->task());
                dbgAssert(cmt::task::State::readyLowPriority == fiber->task()->state());
                dbgAssert(fiber == fiber->task()->fiber());
                return fiber;
            }
        }

        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::f2f(ctx::Fiber* from, ctx::Fiber* to)
    {
        dbgAssert(from);
        dbgAssert(from == _currentFiber);

        dbgAssert(to);
        dbgAssert(to != _currentFiber);
        dbgAssert(to->task());
        dbgAssert(cmt::task::State::work == to->task()->state());
        dbgAssert(!from->task() || cmt::task::State::work != from->task()->state());

        _currentFiber = to;
        from->switchTo(to);
        handleFiberAwake();

        dbgAssert(from == _currentFiber);
        dbgAssert(from->task());
        dbgAssert(cmt::task::State::work == from->task()->state());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::f2r(ctx::Fiber* from)
    {
        dbgAssert(from);
        dbgAssert(from == _currentFiber);
        dbgAssert(!from->task() || cmt::task::State::work != from->task()->state());

        _currentFiber = nullptr;
        from->switchTo(&_rootContext);
        handleFiberAwake();

        dbgAssert(from == _currentFiber);
        dbgAssert(from->task());
        dbgAssert(cmt::task::State::work == from->task()->state());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::r2f(ctx::Fiber* to)
    {
        dbgAssert(!_currentFiber);

        dbgAssert(to);
        dbgAssert(to != _currentFiber);
        dbgAssert(to->task());
        dbgAssert(cmt::task::State::work == to->task()->state());
        dbgAssert(!to->task()->stopRequested());

        _currentFiber = to;
        _rootContext.switchTo(to);
        handleRootAwake();

        dbgAssert(!_currentFiber);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::handleRootAwake()
    {
        dbgAssert(!_currentFiber);
        dbgAssert(!_enumerateFiber);
        while(_enumerateInitiator)
        {
            //enumerate root
            _fiberEnumerationCallback(cmt::task::State::null, _fiberEnumerationCallbackData);

            _rootContext.switchTo(_enumerateInitiator);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Scheduler::handleFiberAwake()
    {
        while(_enumerateInitiator)
        {
            //enumerate
            dbgAssert(_enumerateFiber);
            dbgAssert(_fiberEnumerationCallback);
            _fiberEnumerationCallback(_enumerateFiber->task()->state(), _fiberEnumerationCallbackData);

            ctx::Fiber *prev = _enumerateFiber;
            _enumerateFiber = _enumerateFiber->nextT();
            if(_enumerateFiber == _enumerateInitiator)
            {
                _enumerateFiber = _enumerateFiber->nextT();
            }

            if(_enumerateFiber)
            {
                prev->switchTo(_enumerateFiber, false);
            }
            else
            {
                prev->switchTo(_enumerateInitiator, false);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scheduler Scheduler::_instance{};
}
