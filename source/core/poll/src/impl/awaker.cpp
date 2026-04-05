/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "awaker.hpp"
#include "awaking.hpp"
#include "service.hpp"
#include <dci/cmt/functions.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::poll::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::Awaker(cmt::task::Owner* wokenOwner, cmt::Raisable* raisable, bool keepLoop)
        : _wokenOwner{(wokenOwner ? wokenOwner : &_localWokenOwner)}
        , _raisable{raisable}
        , _keepLoop{keepLoop}
    {
        service.awaking().install(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Awaker::~Awaker()
    {
        service.awaking().uninstall(this);
        _woken->_owner = {};
        _woken.reset();
        _localWokenOwner.stop(false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Awaker::keepLoop() const
    {
        return _keepLoop;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Awaker::woken()
    {
        return _woken->_wire.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Awaker::ready() const
    {
        return _ready.load(std::memory_order_acquire);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::emitWokenIfNeed()
    {
        if(ready() && _woken && _woken->_wire.connected() && !_woken->_inProgress)
        {
            service.awaking().unready(this);
            _ready.store(false, std::memory_order_release);

            _woken->_inProgress = true;
            cmt::spawn() += _wokenOwner * [woken=_woken]
            {
                {
                    dci::utils::AtScopeExit cleaner{[&]{woken->_inProgress=false;}};
                    woken->_wire.in();
                }

                if(woken->_owner)
                    woken->_owner->emitWokenIfNeed();
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::setWokenOwner(cmt::task::Owner* wokenOwner)
    {
        if(wokenOwner != _wokenOwner)
        {
            _wokenOwner->stop(false);
        }
        _wokenOwner = wokenOwner ? wokenOwner : &_localWokenOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::resetWokenOwner()
    {
        _localWokenOwner.stop(false);
        _wokenOwner = &_localWokenOwner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::setRaisable(cmt::Raisable* raisable)
    {
        _raisable = raisable;
        dbgAssert(_raisable);

        if(ready() && _raisable)
        {
            _raisable->raise();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::resetRaisable()
    {
        _raisable = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Awaker::wakeup()
    {
        if(!_ready.exchange(true, std::memory_order_acq_rel))
        {
            service.awaking().ready(this);
        }

        std::error_code ec = service.polling().wakeup();
        (void)ec;
        dbgAssert(!ec);
    }
}
