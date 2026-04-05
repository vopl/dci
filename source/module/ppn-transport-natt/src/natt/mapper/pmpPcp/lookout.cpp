/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "lookout.hpp"
#include "../../mapper.hpp"
#include "../../addr.hpp"

namespace dci::module::ppn::transport::natt::mapper::pmpPcp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::Lookout(Mapper* mapper, bool usePmp, bool usePcp)
        : mapper::netBased::Lookout<Lookout, Service>{mapper}
        , _usePmp{usePmp}
        , _usePcp{usePcp}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::~Lookout()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lookout::usePmp() const
    {
        return _usePmp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lookout::usePcp() const
    {
        return _usePcp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::mcastReceived(Bytes&& data, const net::Endpoint& srvEp)
    {
        std::deque<net::IpAddress> linkAddresses;
        if(srvEp.holds<net::Ip4Endpoint>())
        {
            linkAddresses = linkAddressesFor(srvEp.get<net::Ip4Endpoint>().address);
        }
        else if(srvEp.holds<net::Ip6Endpoint>())
        {
            linkAddresses = linkAddressesFor(srvEp.get<net::Ip6Endpoint>().address);
        }

        for(const net::IpAddress& la : linkAddresses)
        {
            auto iter = _services.emplace(std::piecewise_construct_t{},
                                          std::tie(la, srvEp),
                                          std::forward_as_tuple(this, la, srvEp)).first;

            iter->second.mcastReceived(std::move(data), srvEp);
        }
    }

}
