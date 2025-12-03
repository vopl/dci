// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"

namespace dci::module::net::utils::sockaddr
{
    int family(const api::Endpoint& src);

    socklen_t convert(const api::Endpoint& src, ::sockaddr* dst);
    socklen_t convert(const api::NullEndpoint& src, ::sockaddr* dst);
    socklen_t convert(const api::LocalEndpoint& src, ::sockaddr* dst);
    socklen_t convert(const api::Ip4Endpoint& src, ::sockaddr* dst);
    socklen_t convert(const api::Ip6Endpoint& src, ::sockaddr* dst);


    bool convert(const ::sockaddr* src, socklen_t srcLen, api::Endpoint& dst);
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::NullEndpoint& dst);
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::LocalEndpoint& dst);
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::Ip4Endpoint& dst);
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::Ip6Endpoint& dst);

    bool convert(const ::sockaddr_un* src, socklen_t srcLen, api::NullEndpoint& dst);
    bool convert(const ::sockaddr_un* src, socklen_t srcLen, api::LocalEndpoint& dst);
    bool convert(const ::sockaddr_in* src, socklen_t srcLen, api::Ip4Endpoint& dst);
    bool convert(const ::sockaddr_in6* src, socklen_t srcLen, api::Ip6Endpoint& dst);
}
