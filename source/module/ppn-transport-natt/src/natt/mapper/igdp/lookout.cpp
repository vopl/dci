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

namespace dci::module::ppn::transport::natt::mapper::igdp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::Lookout(Mapper* mapper)
        : mapper::netBased::Lookout<Lookout, Service>{mapper}
    {
        _regularTicker.start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lookout::~Lookout()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Waitable* Lookout::regularTicker()
    {
        return &_regularTicker.waitable();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::regularTick()
    {
        auto sendSearch = [this](const net::Endpoint& ep)
        {
            for(const char* urn : {
                "urn:schemas-upnp-org:service:WANIPConnection:2",
                "urn:schemas-upnp-org:service:WANIPConnection:1",
                "urn:schemas-upnp-org:device:InternetGatewayDevice:2",
                "urn:schemas-upnp-org:device:InternetGatewayDevice:1"})
            {
                poll::WaitableTimer t{std::chrono::milliseconds{50}, false};
                t.start();
                t.wait();

                net::datagram::Channel<> ch = _netHost->datagramChannel().value();

                String request = "M-SEARCH * HTTP/1.1\r\n"
                                 "HOST: " + addr::toString(ep) +"\r\n"
                                 "MAN: \"ssdp:discover\"\r\n"
                                 "MX: 2\r\n"
                                 "ST: "+urn+"\r\n"
                                 "\r\n";

                ch->send(request, ep);

                utils::S2f resp{ch->received()};

                t.interval(std::chrono::milliseconds{250});
                t.start();

                if(0 == cmt::waitAny(resp, t))
                {
                    auto&&[data, srvEp] = resp.detachValue();
                    mcastReceived(std::move(data), std::move(srvEp));
                    break;
                }
            }
        };

        {
            bool needIp4 = false;
            std::set<uint32> needIp6LinkIds;

            for(const net::IpAddress& address : _gateways)
            {
                needIp4 |= address.holds<net::Ip4Address>();

                if(address.holds<net::Ip6Address>())
                {
                    needIp6LinkIds.insert(address.get<net::Ip6Address>().linkId);
                }
            }

            try
            {
                if(needIp4)
                {
                    sendSearch(net::Ip4Endpoint{{_mcastListenAddr4}, _mcastListenPort});
                }

                for(uint32 linkId : needIp6LinkIds)
                {
                    sendSearch(net::Ip6Endpoint{{_mcastListenAddr6, linkId}, _mcastListenPort});
                }
            }
            catch(const cmt::task::Stop&)
            {
            }
            catch(...)
            {
                LOGW("unable to send udp: "<<exception::toString(std::current_exception()));
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lookout::mcastReceived(Bytes&& data, const net::Endpoint& srvEp)
    {
        std::deque<net::IpAddress> linkAddresses;
        if(srvEp.holds<net::Ip4Endpoint>())
        {
            const net::Ip4Endpoint& srvEp4 = srvEp.get<net::Ip4Endpoint>();
            if(_servicePort == srvEp4.port)
            {
                linkAddresses = linkAddressesFor(srvEp4.address);
            }
        }
        else if(srvEp.holds<net::Ip6Endpoint>())
        {
            const net::Ip6Endpoint& srvEp6 = srvEp.get<net::Ip6Endpoint>();
            if(_servicePort == srvEp6.port)
            {
                linkAddresses = linkAddressesFor(srvEp6.address);
            }
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
