// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::natt::addr
{
    std::string toString(const net::Endpoint& v);
    std::string toString(const net::IpEndpoint& v);
    std::string toString(const net::Ip4Endpoint& v);
    std::string toString(const net::Ip6Endpoint& v);

    std::string toString(const net::IpAddress& v);
    std::string toString(const net::Ip4Address& v);
    std::string toString(const net::Ip6Address& v);

    std::string toString(api::Protocol p);
    std::string toString(const net::Endpoint& v, api::Protocol p);
    std::string toString(const net::IpEndpoint& v, api::Protocol p);
    std::string toString(const net::Ip4Endpoint& v, api::Protocol p);
    std::string toString(const net::Ip6Endpoint& v, api::Protocol p);

    std::string toString(const net::IpAddress& v, api::Protocol p);
    std::string toString(const net::Ip4Address& v, api::Protocol p);
    std::string toString(const net::Ip6Address& v, api::Protocol p);

    bool fromString(const std::string& str, api::Protocol& p, net::IpEndpoint& ep);
}
