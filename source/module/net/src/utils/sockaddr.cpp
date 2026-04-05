/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "sockaddr.hpp"

namespace dci::module::net::utils::sockaddr
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int family(const api::Endpoint& src)
    {
        switch(src.index())
        {
        case 1:
            return AF_INET;
        case 2:
            return AF_INET6;
        case 3:
            return AF_UNIX;
        }

        return AF_UNSPEC;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    socklen_t convert(const api::Endpoint& src, ::sockaddr* dst)
    {
        return src.visit([&](const auto& sub)
        {
            return convert(sub, dst);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    socklen_t convert(const api::NullEndpoint&, ::sockaddr* dst)
    {
        memset(dst, 0, sizeof(*dst));
        dst->sa_family = AF_UNSPEC;
        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    socklen_t convert(const api::LocalEndpoint& src, ::sockaddr* dst)
    {
        ::sockaddr_un* dst2 = reinterpret_cast<::sockaddr_un *>(dst);
        memset(dst2, 0, sizeof(*dst2));
        dst2->sun_family = AF_UNIX;

        std::size_t pathLen = std::min(sizeof(dst2->sun_path), src.address.size());

        memcpy(dst2->sun_path, src.address.data(), pathLen);
        return static_cast<socklen_t>(offsetof(::sockaddr_un, sun_path) + pathLen);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    socklen_t convert(const api::Ip4Endpoint& src, ::sockaddr* dst)
    {
        ::sockaddr_in* dst2 = reinterpret_cast<::sockaddr_in *>(dst);
        memset(dst2, 0, sizeof(*dst2));
        dst2->sin_family = AF_INET;
        dst2->sin_port = htons(src.port);
        memcpy(&dst2->sin_addr.s_addr, src.address.octets.data(), sizeof(src.address.octets));

        return sizeof(::sockaddr_in);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    socklen_t convert(const api::Ip6Endpoint& src, ::sockaddr* dst)
    {
        ::sockaddr_in6* dst2 = reinterpret_cast<::sockaddr_in6 *>(dst);
        memset(dst2, 0, sizeof(*dst2));
        dst2->sin6_family = AF_INET6;
        dst2->sin6_port = htons(src.port);
        dst2->sin6_flowinfo = 0;
        memcpy(&dst2->sin6_addr.s6_addr, src.address.octets.data(), sizeof(src.address.octets));
        dst2->sin6_scope_id = src.address.linkId;

        return sizeof(::sockaddr_in6);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::Endpoint& dst)
    {
        switch(src->sa_family)
        {
        case AF_INET:
            return convert(reinterpret_cast<const ::sockaddr_in*>(src), srcLen, dst.sget<api::Ip4Endpoint>());
        case AF_INET6:
            return convert(reinterpret_cast<const ::sockaddr_in6*>(src), srcLen, dst.sget<api::Ip6Endpoint>());
        case AF_UNIX:
            return convert(reinterpret_cast<const ::sockaddr_un*>(src), srcLen, dst.sget<api::LocalEndpoint>());
        }

        dst = api::NullEndpoint{};

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr* src, socklen_t /*srcLen*/, api::NullEndpoint& /*dst*/)
    {
        if(AF_UNSPEC == src->sa_family)
            return true;

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::LocalEndpoint& dst)
    {
        if(AF_UNIX == src->sa_family)
        {
            return convert(reinterpret_cast<const ::sockaddr_un*>(src), srcLen, dst);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::Ip4Endpoint& dst)
    {
        if(AF_INET == src->sa_family)
        {
            return convert(reinterpret_cast<const ::sockaddr_in*>(src), srcLen, dst);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr* src, socklen_t srcLen, api::Ip6Endpoint& dst)
    {
        if(AF_INET6 == src->sa_family)
        {
            return convert(reinterpret_cast<const ::sockaddr_in6*>(src), srcLen, dst);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr_un* src, socklen_t, api::NullEndpoint&)
    {
        dbgAssert(AF_UNSPEC == src->sun_family);
        (void)src;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr_un* src, socklen_t srcLen, api::LocalEndpoint& dst)
    {
        dbgAssert(AF_UNIX == src->sun_family);

        if(srcLen < static_cast<socklen_t>(offsetof(::sockaddr_un, sun_path)))
        {
            return false;
        }

        dst.address.assign(src->sun_path, src->sun_path + srcLen - offsetof(::sockaddr_un, sun_path));
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr_in* src, socklen_t srcLen, api::Ip4Endpoint& dst)
    {
        dbgAssert(AF_INET == src->sin_family);

        if(srcLen < static_cast<socklen_t>(offsetof(::sockaddr_in, sin_addr)))
        {
            return false;
        }

        dst.port = ntohs(src->sin_port);

        if(srcLen >= static_cast<socklen_t>(offsetof(::sockaddr_in, sin_addr) + sizeof(::sockaddr_in::sin_addr)))
        {
            memcpy(dst.address.octets.data(), &src->sin_addr, sizeof(::sockaddr_in::sin_addr));
        }
        else
        {
            memset(dst.address.octets.data(), 0, dst.address.octets.size());
            memcpy(dst.address.octets.data(), &src->sin_addr, srcLen - offsetof(::sockaddr_in, sin_addr));
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool convert(const ::sockaddr_in6* src, socklen_t srcLen, api::Ip6Endpoint& dst)
    {
        dbgAssert(AF_INET6 == src->sin6_family);

        if(srcLen < static_cast<socklen_t>(offsetof(::sockaddr_in6, sin6_addr)))
        {
            return false;
        }

        dst.port = ntohs(src->sin6_port);

        if(srcLen >= static_cast<socklen_t>(offsetof(::sockaddr_in6, sin6_addr) + sizeof(::sockaddr_in6::sin6_addr)))
        {
            memcpy(dst.address.octets.data(), &src->sin6_addr, sizeof(::sockaddr_in6::sin6_addr));
        }
        else
        {
            memset(dst.address.octets.data(), 0, dst.address.octets.size());
            memcpy(dst.address.octets.data(), &src->sin6_addr, srcLen - offsetof(::sockaddr_in6, sin6_addr));
        }

        dst.address.linkId = src->sin6_scope_id;

        return true;
    }
}
