/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "addr.hpp"

namespace dci::module::ppn::transport::natt::addr
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Endpoint& v)
    {
        if(v.holds<net::NullEndpoint>())
        {
            return {};
        }
        if(v.holds<net::Ip4Endpoint>())
        {
            return toString(v.get<net::Ip4Endpoint>());
        }
        if(v.holds<net::Ip6Endpoint>())
        {
            return toString(v.get<net::Ip6Endpoint>());
        }
        if(v.holds<net::LocalEndpoint>())
        {
            return v.get<net::LocalEndpoint>().address;
        }

        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::IpEndpoint& v)
    {
        if(v.holds<net::Ip4Endpoint>())
        {
            return toString(v.get<net::Ip4Endpoint>());
        }

        return toString(v.get<net::Ip6Endpoint>());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip4Endpoint& v)
    {
        return utils::ip::toString(v.address.octets, v.port);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip6Endpoint& v)
    {
        return utils::ip::toString(v.address.octets, v.address.linkId, v.port);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::IpAddress& v)
    {
        if(v.holds<net::Ip4Address>())
        {
            return toString(v.get<net::Ip4Address>());
        }

        return toString(v.get<net::Ip6Address>());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip4Address& v)
    {
        return utils::ip::toString(v.octets);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip6Address& v)
    {
        return utils::ip::toString(v.octets, v.linkId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(api::Protocol p)
    {
        if(api::Protocol::udp == p)
        {
            return "udp";
        }

        return "tcp";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Endpoint& v, api::Protocol p)
    {
        if(v.holds<net::NullEndpoint>())
        {
            return "null://";
        }
        if(v.holds<net::Ip4Endpoint>())
        {
            return toString(v.get<net::Ip4Endpoint>(), p);
        }
        if(v.holds<net::Ip6Endpoint>())
        {
            return toString(v.get<net::Ip6Endpoint>(), p);
        }
        if(v.holds<net::LocalEndpoint>())
        {
            return "local://" + v.get<net::LocalEndpoint>().address;
        }
        return "unknown://";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::IpEndpoint& v, api::Protocol p)
    {
        if(v.holds<net::Ip4Endpoint>())
        {
            return toString(v.get<net::Ip4Endpoint>(), p);
        }

        return toString(v.get<net::Ip6Endpoint>(), p);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip4Endpoint& v, api::Protocol p)
    {
        return toString(p) + "4://" + toString(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip6Endpoint& v, api::Protocol p)
    {
        return toString(p) + "6://" + toString(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::IpAddress& v, api::Protocol p)
    {
        if(v.holds<net::Ip4Address>())
        {
            return toString(v.get<net::Ip4Address>(), p);
        }

        return toString(v.get<net::Ip6Address>(), p);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip4Address& v, api::Protocol p)
    {
        return toString(p) + "4://" + toString(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const net::Ip6Address& v, api::Protocol p)
    {
        return toString(p) + "6://[" + toString(v) + "]";
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(const std::string& str, api::Protocol& p, net::IpEndpoint& ep)
    {
        utils::URI<> uri;
        if(!utils::uri::parse(str, uri))
            return false;

        return std::visit([&]<class Alt>(const Alt& alt)
                          {
                              if constexpr(std::is_same_v<utils::uri::TCP4<>, Alt> || std::is_same_v<utils::uri::TCP6<>, Alt>)
                                  p = api::Protocol::tcp;
                              else if constexpr(std::is_same_v<utils::uri::UDP4<>, Alt> || std::is_same_v<utils::uri::UDP6<>, Alt>)
                                  p = api::Protocol::udp;
                              else
                                  return false;

                              if constexpr(std::is_same_v<utils::uri::TCP4<>, Alt> || std::is_same_v<utils::uri::UDP4<>, Alt>)
                              {
                                  net::Ip4Endpoint& ep4 = ep.sget<net::Ip4Endpoint>();
                                  if(alt._auth._port)
                                      utils::ip::fromString(*alt._auth._port, ep4.port);
                                  return utils::ip::fromString(utils::uri::host(alt), ep4.address.octets);
                              }
                              else if constexpr(std::is_same_v<utils::uri::TCP6<>, Alt> || std::is_same_v<utils::uri::UDP6<>, Alt>)
                              {
                                  net::Ip6Endpoint& ep6 = ep.sget<net::Ip6Endpoint>();
                                  if(alt._auth._port)
                                      utils::ip::fromString(*alt._auth._port, ep6.port);
                                  return utils::ip::fromString(utils::uri::host(alt), ep6.address.octets, ep6.address.linkId);
                              }

                              return false;
                          }, uri);
    }
}
