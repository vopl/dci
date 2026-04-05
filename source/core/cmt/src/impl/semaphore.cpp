/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "semaphore.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Semaphore::Semaphore(std::size_t depth)
        : Lockable(
              [](const Waitable* w){ return static_cast<const Semaphore*>(w)->canLock(); },
              [](Waitable* w){ return static_cast<Semaphore*>(w)->tryLock(); },
              [](Lockable* l){ return static_cast<Semaphore*>(l)->unlock(); })
        , _depth(depth)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Semaphore::~Semaphore()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::tryDestruction(Semaphore* s)
    {
        s->~Semaphore();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Semaphore::canLock() const
    {
        return _counter < _depth;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Semaphore::tryLock()
    {
        throwTaskStopIfNeed();

        if(_counter < _depth)
        {
            _counter++;
            return true;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::lock()
    {
        if(tryLock())
        {
            return;
        }

        dbgAssert(_counter == _depth);

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::unlock()
    {
        if(!_counter)
        {
            return;
        }
        _counter--;

        _links.each([this](WWLink* link)
        {
            _counter++;
            if(link->_waiter->readyOffer(link))
            {
                dbgAssert(_counter <= _depth);
                if(_counter >= _depth)
                {
                    return false;
                }
            }
            else
            {
                _counter--;
            }
            return true;
        });

        _links.each([this](WWLink* link)
        {
            if(canLock())
            {
                return false;
            }
            link->_waiter->readyOffer<false>(link);
            return true;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Semaphore::wait()
    {
        return lock();
    }

}
