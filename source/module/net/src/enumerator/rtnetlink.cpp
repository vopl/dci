/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "rtnetlink.hpp"
#include "../links.hpp"
#include "../routes.hpp"
#include "dci/poll/descriptor/native.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

namespace dci::module::net::enumerator
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Rtnetlink::Rtnetlink(Links* links, Routes* routes)
        : _links{links}
        , _routes{routes}
    {
        //socket
        dbgAssert(!_sock);
        _sock.reset(new poll::Descriptor{
                        poll::descriptor::Native{socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)},
                        [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState)
                        {
                            onSock(native, readyState);
                        }});
        if(!_sock->valid())
        {
            _sock.reset();
            LOGE("rtnetlink invalid socket");
            return;
        }

        //bind
        memset(&_address, 0, sizeof(_address));
        _address.nl_family = AF_NETLINK;
        _address.nl_pid = static_cast<__u32>(getpid());
        _address.nl_groups = 0
                | RTMGRP_LINK
                | RTMGRP_IPV4_IFADDR
                | RTMGRP_IPV4_MROUTE | RTMGRP_IPV4_ROUTE
                | RTMGRP_IPV6_IFADDR
                | RTMGRP_IPV6_MROUTE | RTMGRP_IPV6_ROUTE
                             ;

        if(0 != bind(_sock->native(), reinterpret_cast<sockaddr*>(&_address), sizeof(_address)))
        {
            LOGE("rtnetlink bind: "<<strerror(errno));
            _sock.reset();
            return;
        }

        //message area
        memset(&_msgiov, 0, sizeof(_msgiov));
        _msgiov.iov_len = 8192;
        _msgiov.iov_base = malloc(_msgiov.iov_len);
        if(!_msgiov.iov_base)
        {
            LOGE("rtnetlink malloc null");
            _sock.reset();
            return;
        }
        memset(_msgiov.iov_base, 0, _msgiov.iov_len);

        memset(&_msg, 0, sizeof(_msg));
//        _msg.msg_name = &_address;
//        _msg.msg_namelen = sizeof(_address);
        _msg.msg_iov = &_msgiov;
        _msg.msg_iovlen = 1;

        //initialize
        doNextRequest();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Rtnetlink::~Rtnetlink()
    {
        if(_sock)
        {
            _sock->close();
            _sock.reset();
        }

        if(_msgiov.iov_base)
        {
            free(_msgiov.iov_base);
            _msgiov.iov_base = nullptr;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Rtnetlink::doNextRequest()
    {
        if(! (_state & sf_requestLink))
        {
            request(RTM_GETLINK);
            return;
        }

        if(! (_state & sf_requestAddr))
        {
            request(RTM_GETADDR);
            return;
        }

        if(! (_state & sf_requestRoute))
        {
            request(RTM_GETROUTE);
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Rtnetlink::request(uint32 type)
    {
        struct nl_req_t
        {
            nlmsghdr hdr;
            rtgenmsg gen;
        };

        nl_req_t req;
        memset(&req, 0, sizeof(req));
        req.hdr.nlmsg_len   = NLMSG_LENGTH(sizeof(rtgenmsg));
        req.hdr.nlmsg_type  = static_cast<__u16>(type);
        req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
        req.hdr.nlmsg_seq   = 1;
        req.hdr.nlmsg_pid   = static_cast<__u32>(getpid());
        req.gen.rtgen_family= AF_PACKET;

        iovec iov;
        iov.iov_base    = &req;
        iov.iov_len     = req.hdr.nlmsg_len;

        sockaddr_nl address;
        memset(&address, 0, sizeof(address));
        address.nl_family = AF_NETLINK;

        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov     = &iov;
        msg.msg_iovlen  = 1;
        msg.msg_name    = &address;
        msg.msg_namelen = sizeof(address);

        dbgAssert(_sock);
        if(0 > sendmsg(_sock->native(), &msg, 0))
        {
            LOGE("rtnetlink send: "<<strerror(errno));
            return false;
        }

        switch(type)
        {
        case RTM_GETLINK:   _state |= sf_requestLink; break;
        case RTM_GETADDR:   _state |= sf_requestAddr; break;
        case RTM_GETROUTE:  _state |= sf_requestRoute; break;
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        struct LinkAttrs
        {
            List<uint8>     _hwAddress;
            bool            _hwAddressFetched{false};

            std::string     _name;
            bool            _nameFetched{false};

            uint32          _mtu{0};
            bool            _mtuFetched{false};

            bool            _wireless{false};
            bool            _wirelessFetched{false};
        };

        void fetchRta(rtattr* rta, uint32 len, LinkAttrs& res)
        {
            while(RTA_OK(rta, len))
            {
                switch(rta->rta_type)
                {
                case IFLA_ADDRESS:
                    {
                        const uint8* data = static_cast<const uint8 *>(RTA_DATA(rta));
                        res._hwAddress.assign(data, data+RTA_PAYLOAD(rta));
                    }
                    res._hwAddressFetched = true;
                    break;
                case IFLA_IFNAME:
                    res._name = reinterpret_cast<char *>(RTA_DATA(rta));
                    res._nameFetched = true;
                    break;
                case IFLA_MTU:
                    res._mtu = *reinterpret_cast<uint32 *>(RTA_DATA(rta));
                    res._mtuFetched = true;
                    break;
                case IFLA_WIRELESS:
                    res._wireless = true;
                    res._wirelessFetched = true;
                    break;
                }

                rta = RTA_NEXT(rta, len);
            }
        }

        void fetchRta(rtattr* rta, uint32 len, api::link::Ip4Address& address, uint8 prefixlen)
        {
            memset(address.address.octets.data(), 0, 4);
            memset(address.netmask.octets.data(), 0, 4);
            memset(address.broadcast.octets.data(), 0, 4);

            for(uint8 i(0); i<prefixlen && i<32; i++)
            {
                address.netmask.octets[i/8] |= (1<<i%8);
            }
            address.prefixLength = prefixlen;

            while(RTA_OK(rta, len))
            {
                switch(rta->rta_type)
                {
                case IFA_LOCAL:
                    memcpy(address.address.octets.data(), RTA_DATA(rta), 4);
                    address.netmask.octets = dci::utils::ip::masked(address.address.octets, address.prefixLength);
                    break;
                case IFA_BROADCAST:
                    memcpy(address.broadcast.octets.data(), RTA_DATA(rta), 4);
                    break;
                }

                rta = RTA_NEXT(rta, len);
            }
        }

        void fetchRta(rtattr* rta, uint32 len, api::link::Ip6Address& address, uint8_t prefixlen, uint8_t scope)
        {
            address.prefixLength = prefixlen;

            switch(scope)
            {
            case RT_SCOPE_UNIVERSE:
                address.scope = api::link::Ip6AddressScope::global;
                break;
            case RT_SCOPE_SITE:
                address.scope = api::link::Ip6AddressScope::site;
                break;
            case RT_SCOPE_LINK:
                address.scope = api::link::Ip6AddressScope::link;
                break;
            case RT_SCOPE_HOST:
                address.scope = api::link::Ip6AddressScope::host;
                break;
            case RT_SCOPE_NOWHERE:
            default:
                address.scope = api::link::Ip6AddressScope::null;
                break;
            }

            memset(address.address.octets.data(), 0, address.address.octets.size());

            while(RTA_OK(rta, len))
            {
                switch(rta->rta_type)
                {
                case IFA_ADDRESS:
                    memcpy(address.address.octets.data(), RTA_DATA(rta), address.address.octets.size());
                    break;
                }

                rta = RTA_NEXT(rta, len);
            }
        }

        void fetchRta(rtattr* rta, uint32 len, const rtmsg& rt, api::route::Entry4& e4)
        {
            e4.dst.octets.fill(0);
            e4.dstPrefixLength = rt.rtm_dst_len;
            e4.nextHop.octets.fill(0);
            e4.linkId = ~uint32{};

            while(RTA_OK(rta, len))
            {
                std::size_t rtaLen = RTA_PAYLOAD(rta);

                switch(rta->rta_type)
                {
                case RTA_DST:
                    if(rtaLen <= 4)
                    {
                        memcpy(e4.dst.octets.data(), RTA_DATA(rta), rtaLen);
                    }
                    break;
                case RTA_GATEWAY:
                    if(rtaLen <= 4)
                    {
                        memcpy(e4.nextHop.octets.data(), RTA_DATA(rta), rtaLen);
                    }
                    break;
                case RTA_OIF:
                    if(rtaLen == sizeof(e4.linkId))
                    {
                        memcpy(&e4.linkId, RTA_DATA(rta), rtaLen);
                    }
                    break;
                }

                rta = RTA_NEXT(rta, len);
            }
        }

        void fetchRta(rtattr* rta, uint32 len, const rtmsg& rt, api::route::Entry6& e6)
        {
            e6.dst.octets.fill(0);
            e6.dstPrefixLength = rt.rtm_dst_len;
            e6.nextHop.octets.fill(0);
            e6.linkId = ~uint32{};

            while(RTA_OK(rta, len))
            {
                std::size_t rtaLen = RTA_PAYLOAD(rta);

                switch(rta->rta_type)
                {
                case RTA_DST:
                    if(rtaLen <= 16)
                    {
                        memcpy(e6.dst.octets.data(), RTA_DATA(rta), rtaLen);
                    }
                    break;
                case RTA_GATEWAY:
                    if(rtaLen <= 16)
                    {
                        memcpy(e6.nextHop.octets.data(), RTA_DATA(rta), rtaLen);
                    }
                    break;
                case RTA_OIF:
                    if(rtaLen == sizeof(e6.linkId))
                    {
                        memcpy(&e6.linkId, RTA_DATA(rta), rtaLen);
                    }
                    break;
                }

                rta = RTA_NEXT(rta, len);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Rtnetlink::onSock(int fd, poll::descriptor::ReadyStateFlags readyState)
    {
        if(poll::descriptor::rsf_error & readyState)
        {
            LOGE("rtnetlink poll: "<<_sock->error());
            return;
        }

        if(!(poll::descriptor::rsf_read & readyState))
        {
            return;
        }

        bool someForLinks{};
        bool someForRoutes{};

        for(;;)
        {
            ssize_t readedSSize = recvmsg(fd, &_msg, MSG_DONTWAIT);

            if(0 >= readedSSize)
            {
                if(EINTR == errno || EAGAIN == errno)
                {
                    break;
                }

                LOGE("rtnetlink recvmsg: "<<strerror(errno));
                break;
            }
            size_t readedSize = static_cast<size_t>(readedSSize);

            for(
                nlmsghdr* h = static_cast<nlmsghdr *>(_msgiov.iov_base);
                NLMSG_OK(h, readedSize);
                h = NLMSG_NEXT(h, readedSize))
            {
                switch (h->nlmsg_type)
                {
                case NLMSG_DONE:
                    {
                        doNextRequest();
                    }
                    break;

                case RTM_NEWLINK:
                    _state |= sf_responseLink;
                    someForLinks = true;
                    {
                        ifinfomsg* ifi = static_cast<ifinfomsg*>(NLMSG_DATA(h));
                        uint32 id = static_cast<uint32>(ifi->ifi_index);
                        Link* link = _links->getLink(id);
                        bool isNew = !link;

                        LinkAttrs attrs;
                        fetchRta(IFLA_RTA(ifi), h->nlmsg_len, attrs);

                        if(!link)
                        {
                            if(!ifi->ifi_change && attrs._wireless)
                            {
                                //ignore wireless events for absent devices
                                continue;
                            }

                            link = _links->allocLink(id);
                        }

                        if(attrs._hwAddressFetched)
                        {
                            link->setHwAddress(attrs._hwAddress);
                        }

                        if(attrs._nameFetched)
                        {
                            link->setName(attrs._name);
                        }

                        if(attrs._mtuFetched)
                        {
                            link->setMtu(attrs._mtu);
                        }

                        {
                            api::link::Flags flags {};

                            if(ifi->ifi_flags & IFF_UP)         flags |= static_cast<uint32>(api::link::Flags::up);
                            if(ifi->ifi_flags & IFF_RUNNING)    flags |= static_cast<uint32>(api::link::Flags::running);
                            if(ifi->ifi_flags & IFF_BROADCAST)  flags |= static_cast<uint32>(api::link::Flags::broadcast);
                            if(ifi->ifi_flags & IFF_LOOPBACK)   flags |= static_cast<uint32>(api::link::Flags::loopback);
                            if(ifi->ifi_flags & IFF_POINTOPOINT)flags |= static_cast<uint32>(api::link::Flags::p2p);
                            if(ifi->ifi_flags & IFF_MULTICAST)  flags |= static_cast<uint32>(api::link::Flags::multicast);

                            link->setFlags(flags);
                        }

                        if(isNew)
                        {
                            _links->allocatedLinkInitialized(id, link);
                        }
                    }
                    break;

                case RTM_DELLINK:
                    _state |= sf_responseLink;
                    someForLinks = true;
                    {
                        ifinfomsg* ifi = static_cast<ifinfomsg*>(NLMSG_DATA(h));
                        _links->delLink(static_cast<uint32>(ifi->ifi_index));
                    }
                    break;

                case RTM_NEWADDR:
                case RTM_DELADDR:
                    _state |= sf_responseAddr;
                    someForLinks = true;
                    {
                        ifaddrmsg* ifa = static_cast<ifaddrmsg *>(NLMSG_DATA(h));
                        uint32 linkId = static_cast<uint32>(ifa->ifa_index);
                        Link* link = _links->getLink(linkId);

                        if(!link)
                        {
                            break;
                        }

                        switch(ifa->ifa_family)
                        {
                        case AF_INET:
                            {
                                api::link::Ip4Address address;
                                fetchRta(IFA_RTA(ifa), h->nlmsg_len, address, ifa->ifa_prefixlen);

                                if(RTM_NEWADDR == h->nlmsg_type)
                                {
                                    link->addIp4(address);
                                }
                                else
                                {
                                    link->delIp4(address);
                                }
                            }
                            break;
                        case AF_INET6:
                            {
                                api::link::Ip6Address address;
                                fetchRta(IFA_RTA(ifa), h->nlmsg_len, address, ifa->ifa_prefixlen, ifa->ifa_scope);

                                if(api::link::Ip6AddressScope::link == address.scope)
                                {
                                    address.address.linkId = linkId;
                                }
                                else
                                {
                                    address.address.linkId = 0;
                                }

                                if(RTM_NEWADDR == h->nlmsg_type)
                                {
                                    link->addIp6(address);
                                }
                                else
                                {
                                    link->delIp6(address);
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    break;

                case RTM_NEWROUTE:
                case RTM_DELROUTE:
                    _state |= sf_responseRoute;
                    someForRoutes = true;
                    {
                        rtmsg* rt = static_cast<rtmsg *>(NLMSG_DATA(h));

                        if(rt->rtm_table != RT_TABLE_MAIN)
                        {
                            break;
                        }

                        switch(rt->rtm_family)
                        {
                        case AF_INET:
                            {
                                api::route::Entry4 e4;
                                fetchRta(RTM_RTA(rt), h->nlmsg_len, *rt, e4);

                                if(RTM_NEWROUTE == h->nlmsg_type)
                                {
                                    _routes->add(e4);
                                }
                                else
                                {
                                    _routes->del(e4);
                                }
                            }
                            break;
                        case AF_INET6:
                            {
                                api::route::Entry6 e6;
                                fetchRta(RTM_RTA(rt), h->nlmsg_len, *rt, e6);

                                if(RTM_NEWROUTE == h->nlmsg_type)
                                {
                                    _routes->add(e6);
                                }
                                else
                                {
                                    _routes->del(e6);
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }
                    break;

                default:
                    break;
                }
            }
        }

        if(someForLinks)
        {
            _links->flushChanges((_state & sf_responseLink) && (_state & sf_responseAddr));
        }

        if(someForRoutes)
        {
            _routes->flushChanges((_state & sf_responseRoute));
        }
    }
}

#pragma GCC diagnostic pop
