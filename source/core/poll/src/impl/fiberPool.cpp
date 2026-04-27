/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "fiberPool.hpp"
#include <dci/utils/atScopeExit.hpp>

namespace dci::poll::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    FiberPool::FiberPool()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    FiberPool::~FiberPool()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void FiberPool::start()
    {
        if(_stop)
            _stop = false;

        spawnFiberIfNeed();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> FiberPool::execInFiber()
    {
        return _execInFiber.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void FiberPool::needExecInFiber()
    {
        _needExecInFiber.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool FiberPool::spawnFiberIfNeed()
    {
        if(_stop || _fibersCount > _busyCount)
            return false;

        ++_fibersCount;
        cmt::spawn() += _tol * [this]{ fiber(); };
        return true;
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void FiberPool::stop()
    {
        _stop = true;
        _needExecInFiber.raise();
        _tol.flush(false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void FiberPool::fiber()
    {
        auto cleaner{utils::AtScopeExit{[&]
        {
            --_fibersCount;
            if(_stop)
                _needExecInFiber.raise();
        }}};

        std::size_t busyCountInitial{_busyCount};
        while(!_stop && busyCountInitial <= _busyCount)
        {
            _needExecInFiber.wait();

            ++_busyCount;
            auto unbusy{utils::AtScopeExit{[&]{ --_busyCount; }}};

            _execInFiber.in();
        }
    }
}
