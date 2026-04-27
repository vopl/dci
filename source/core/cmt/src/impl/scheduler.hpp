/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "task/body.hpp"
#include "ctx/root.hpp"
#include "ctx/fiber.hpp"

namespace dci::cmt::task
{
    class Face;
}

namespace dci::cmt::impl
{
    class Scheduler
    {
    public:
        Scheduler();
        ~Scheduler();

    public:
        static Scheduler& instance();

    public:
        void spawnTask(task::Body* task);

        enum ReadyFiberKind: std::uint32_t
        {
            rfk_regular             = 0x01,
            rfk_new4SpawnedTasks    = 0x02,
            rfk_lowPriority         = 0x04,
            rfk_4Stop               = 0x08,
            rfk_any                 = 0x0f,
        };
        bool yield(std::uint32_t rfk = rfk_any);
        void hold();
        void ready(ctx::Fiber* fiber);
        std::size_t executeReadyFibers();
        bool switchTo(task::Body* task);

        using FiberEnumerationCallback = void(*)(cmt::task::State, void*);
        void enumerateFibers(FiberEnumerationCallback, void*);

    public:
        ctx::Fiber* currentFiber();
        task::Body* currentTask();

        void fiberStarted();
        void taskCompleted();

    private:
        ctx::Fiber* dequeueReadyFiber(std::uint32_t rfk = rfk_any);
        void f2f(ctx::Fiber* from, ctx::Fiber* to);
        void f2r(ctx::Fiber* from);
        void r2f(ctx::Fiber* to);

    private:
        void handleRootAwake();
        void handleFiberAwake();

    private:
        static Scheduler    _instance;
        ctx::Root           _rootContext;
        ctx::Fiber*         _currentFiber;

        utils::IntrusiveDlist<ctx::Fiber, Scheduler> _hold;
        utils::IntrusiveDlist<ctx::Fiber, Scheduler> _empty;
        utils::IntrusiveDlist<ctx::Fiber, Scheduler> _ready4Stop;
        utils::IntrusiveDlist<ctx::Fiber, Scheduler> _ready;
        utils::IntrusiveDlist<task::Body, Scheduler> _spawnedTasks;
        utils::IntrusiveDlist<ctx::Fiber, Scheduler> _readyLowPriority;

    private:
        ctx::Fiber*                 _enumerateInitiator{};
        ctx::Fiber*                 _enumerateFiber{};
        FiberEnumerationCallback    _fiberEnumerationCallback {};
        void*                       _fiberEnumerationCallbackData {};
    };
}
