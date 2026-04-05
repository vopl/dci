/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "owner.hpp"
#include "subscription.hpp"
#include "box.hpp"
#include <dci/sbs/subscription.hpp>

#include <dci/utils/dbg.hpp>
#include <iostream>

namespace dci::sbs::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::Owner()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner::~Owner()
    {
        flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::flush()
    {
        while(_first)
        {
            dbgAssert(this == _first->_owner);

            if(_first->_box)
            {
                _first->_box->removeAndDelete(_first);
            }
            else
            {
                _first->_activator(himpl::impl2Face<sbs::Subscription>(_first), nullptr, sbs::Subscription::del);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::push(Subscription* subscription)
    {
        dbgAssert(this == subscription->_owner);

        if(!_first)
        {
            dbgAssert(!_last);

            subscription->_nextInOwner = subscription->_prevInOwner = nullptr;

            _first = _last = subscription;
        }
        else
        {
            dbgAssert(_last);
            dbgAssert(!_first->_prevInOwner);
            dbgAssert(!_last->_nextInOwner);

            subscription->_nextInOwner = nullptr;
            subscription->_prevInOwner = _last;

            _last->_nextInOwner = subscription;
            _last = subscription;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Owner::remove(Subscription* subscription)
    {
        if(_first == subscription)
        {
            dbgAssert(!subscription->_prevInOwner);

            if(subscription->_nextInOwner)
            {
                dbgAssert(_last != subscription);
                dbgAssert(subscription->_nextInOwner->_prevInOwner == subscription);

                subscription->_nextInOwner->_prevInOwner = nullptr;
                _first = subscription->_nextInOwner;
                subscription->_nextInOwner = nullptr;
            }
            else
            {
                dbgAssert(_last == subscription);

                _first = _last = nullptr;
            }
        }
        else if(_last == subscription)
        {
            dbgAssert(!subscription->_nextInOwner);
            dbgAssert(subscription->_prevInOwner);
            dbgAssert(subscription->_prevInOwner->_nextInOwner == subscription);

            subscription->_prevInOwner->_nextInOwner = nullptr;
            _last = subscription->_prevInOwner;

            subscription->_prevInOwner = nullptr;
        }
        else
        {
            dbgAssert(subscription->_prevInOwner);
            dbgAssert(subscription->_prevInOwner->_nextInOwner == subscription);

            dbgAssert(subscription->_nextInOwner);
            dbgAssert(subscription->_nextInOwner->_prevInOwner == subscription);

            subscription->_prevInOwner->_nextInOwner = subscription->_nextInOwner;
            subscription->_nextInOwner->_prevInOwner = subscription->_prevInOwner;

            subscription->_prevInOwner = nullptr;
            subscription->_nextInOwner = nullptr;
        }

        subscription->_owner = nullptr;
    }

}
