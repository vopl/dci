/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "performer.hpp"
#include "service.hpp"
#include "../../addr.hpp"

namespace dci::module::ppn::transport::natt::mapper::pmpPcp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::Performer(Service* srv, api::Protocol protocol, uint16 internalPort, const net::IpEndpoint& externalEp)
        : _srv{srv}
        , _protocol{protocol}
        , _internalPort{internalPort}
        , _externalEp{externalEp}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Performer::~Performer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::start()
    {
        bool res = _srv && _srv->map(_externalEp, _internalPort, _protocol, std::chrono::seconds{60*10});
        if(res)
        {
            _external.value = addr::toString(_externalEp, _protocol);
        }
        else
        {
            _external.value.clear();
        }
        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Performer::keepalive()
    {
        if(!_srv) return false;
        poll::WaitableTimer t{std::chrono::seconds{60*1}};
        t.start();
        cmt::waitAny(t.waitable(), _srv->serviceChangedWaitable());
        return start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Performer::stop()
    {
        if(_srv && !_external.value.empty())
        {
            _srv->map(_externalEp, _internalPort, _protocol, std::chrono::seconds{});
            _external.value.clear();
        }
    }
}
