/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
