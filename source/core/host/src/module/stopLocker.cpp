/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/host/module/entry.hpp>
#include <dci/host/module/stopLocker.hpp>

namespace dci::host::module
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::~StopLocker()
    {
        if(_e)
        {
            _e->stopLockCounterDec();
            _e = nullptr;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker(Entry* e)
        : _e{e}
    {
        dbgAssert(_e);
        if(_e)
        {
            _e->stopLockCounterInc();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker(StopLocker&& from)
        : _e{std::exchange(from._e, {})}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker::StopLocker(const StopLocker& from)
        : _e{from._e}
    {
        if(_e)
        {
            _e->stopLockCounterInc();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker& StopLocker::operator=(StopLocker&& from)
    {
        if(_e == from._e)
        {
            if(from._e)
            {
                from._e->stopLockCounterDec();
                from._e = nullptr;
            }
            return *this;
        }

        if(_e)
        {
            _e->stopLockCounterDec();
        }

        _e = from._e;
        from._e = nullptr;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StopLocker& StopLocker::operator=(const StopLocker& from)
    {
        if(_e == from._e)
        {
            return *this;
        }

        if(from._e)
        {
            from._e->stopLockCounterInc();
        }

        if(_e)
        {
            _e->stopLockCounterDec();
        }

        _e = from._e;

        return *this;
    }
}
