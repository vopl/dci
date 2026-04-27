/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "fiberPool.hpp"
#include "polling.hpp"
#include "clocking.hpp"
#include "awaking.hpp"
#include <system_error>
#include <dci/sbs/wire.hpp>

namespace dci::poll::impl
{
    class Service
    {
    public:
        Service();
        ~Service();

    public:
        std::error_code     initialize();
        std::error_code     run(bool emitStartedStopped);
        sbs::Signal<>       started();
        std::error_code     stop();
        sbs::Signal<>       stopped();
        std::error_code     deinitialize();

    private:
        friend class Timer;
        Clocking& clocking();

    private:
        friend class Descriptor;
        Polling& polling();

    private:
        friend class Awaker;
        Awaking& awaking();

    private:
        FiberPool   _fiberPool;
        Clocking    _clocking;
        Polling     _polling;
        Awaking     _awaking;

        sbs::Wire<>     _started;
        sbs::Wire<>     _stopped;
        bool            _stop{true};
    };

    extern Service& service;
}
