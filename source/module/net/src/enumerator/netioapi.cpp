/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "netioapi.hpp"
#include "../links.hpp"
#include "../routes.hpp"
#include "../utils/sockaddr.hpp"

namespace dci::module::net::enumerator
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Netioapi::Netioapi(Links* links, Routes* routes)
        : _links{links}
        , _routes{routes}
    {
        if(DWORD res = NotifyIpInterfaceChange(
            AF_UNSPEC,              //[in]      ADDRESS_FAMILY               Family,
            [](PVOID ctx, PMIB_IPINTERFACE_ROW row, MIB_NOTIFICATION_TYPE ntype)
            {
                Netioapi* self = static_cast<Netioapi*>(ctx);
                {
                    std::scoped_lock lock{self->_mtx};
                    self->_linkChanges.push_back({row->InterfaceIndex, row->InterfaceLuid, ntype});
                }
                self->_awaker.wakeup();
            },                      //[in]      PIPINTERFACE_CHANGE_CALLBACK Callback,
            this,                   //[in]      PVOID                        CallerContext,
            FALSE,                  //[in]      BOOLEAN                      InitialNotification,
            &_hIpInterfaceChange    //[in, out] HANDLE                       *NotificationHandle
            ))
        {
            LOGE("NotifyIpInterfaceChange failed: " << dci::utils::win32::error::make(res));
        }

        if(DWORD res = NotifyRouteChange2(
            AF_UNSPEC,      //[in]      ADDRESS_FAMILY             AddressFamily,
            [](PVOID ctx, PMIB_IPFORWARD_ROW2 row, MIB_NOTIFICATION_TYPE ntype)
            {
                Netioapi* self = static_cast<Netioapi*>(ctx);
                {
                    std::scoped_lock lock{self->_mtx};
                    self->_routeChanges.push_back({*row, ntype});
                }
                self->_awaker.wakeup();
            },              //[in]      PIPFORWARD_CHANGE_CALLBACK Callback,
            this,           //[in]      PVOID                      CallerContext,
            FALSE,          //[in]      BOOLEAN                    InitialNotification,
            &_hRouteChange  //[in, out] HANDLE                     *NotificationHandle
            ))
        {
            LOGE("NotifyIpInterfaceChange failed: " << dci::utils::win32::error::make(res));
        }

        {
            PMIB_IPINTERFACE_TABLE table;
            if(DWORD res = GetIpInterfaceTable(AF_UNSPEC, &table))
            {
                LOGE("GetIpInterfaceTable failed: " << dci::utils::win32::error::make(res));
            }
            else
            {
                std::shared_ptr<LinkChangeCommon> cmn;
                for(ULONG idx{}; idx<table->NumEntries; ++idx)
                {
                    LinkChange change;
                    change._linkId = table->Table[idx].InterfaceIndex;
                    change._linkLuid = table->Table[idx].InterfaceLuid;
                    change._ntype = MibAddInstance;
                    onLinkChange(change, cmn);
                }
                FreeMibTable(table);
            }
        }

        {
            PMIB_IPFORWARD_TABLE2 table;
            if(DWORD res = GetIpForwardTable2(AF_UNSPEC, &table))
            {
                LOGE("GetIpForwardTable2 failed: " << dci::utils::win32::error::make(res));
            }
            else
            {
                for(ULONG idx{}; idx<table->NumEntries; ++idx)
                {
                    RouteChange change;
                    change._row = table->Table[idx];
                    change._ntype = MibAddInstance;
                    onRouteChange(change);
                }
                FreeMibTable(table);
                _routes->flushChanges(true);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Netioapi::~Netioapi()
    {
        if(_hIpInterfaceChange)
        {
            CancelMibChangeNotify2(_hIpInterfaceChange);
            _hIpInterfaceChange = NULL;
        }

        if(_hRouteChange)
        {
            CancelMibChangeNotify2(_hRouteChange);
            _hRouteChange = NULL;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct Netioapi::LinkChangeCommon
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        LinkChangeCommon(Links* links)
            : _links{links}
        {
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        ~LinkChangeCommon()
        {
            _links->flushChanges(true);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        List<api::link::Ip4Address> ip4For(uint32 ifId)
        {
            List<api::link::Ip4Address> res;

            if(!ensureAddressesFetched() || _addressesBuf.empty())
            {
                return res;
            }

            PIP_ADAPTER_ADDRESSES addr = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(&_addressesBuf[0]);
            while(addr)
            {
                if(ifId == addr->IfIndex && IP_ADAPTER_IPV4_ENABLED & addr->Flags)
                {
                    api::link::Ip4Address resAddr{};
                    bool addressValid{};

                    PIP_ADAPTER_UNICAST_ADDRESS uaddr = addr->FirstUnicastAddress;
                    while(uaddr)
                    {
                        if (IP_DAD_STATE::IpDadStatePreferred == uaddr->DadState && AF_INET == uaddr->Address.lpSockaddr->sa_family && 32 >= uaddr->OnLinkPrefixLength)
                        {
                            api::Ip4Endpoint ep;
                            addressValid = utils::sockaddr::convert(uaddr->Address.lpSockaddr, uaddr->Address.iSockaddrLength, ep);
                            if(addressValid)
                            {
                                resAddr.address = ep.address;
                                resAddr.prefixLength = uaddr->OnLinkPrefixLength;
                                break;
                            }
                        }
                        uaddr = uaddr->Next;
                    }

                    if(addressValid)
                    {
                        PIP_ADAPTER_PREFIX_XP prefix = addr->FirstPrefix;
                        std::size_t cnt{};
                        while(prefix)
                        {
                            SOCKET_ADDRESS socketAddress = prefix->Address;

                            api::Ip4Endpoint ip4Endpoint;
                            if(utils::sockaddr::convert(socketAddress.lpSockaddr, socketAddress.iSockaddrLength, ip4Endpoint))
                            {
                                switch(cnt)
                                {
                                case 0:
                                    resAddr.netmask.octets = dci::utils::ip::masked(ip4Endpoint.address.octets, prefix->PrefixLength);
                                    dbgAssert(resAddr.prefixLength == prefix->PrefixLength);
                                    break;

                                case 1:
                                    dbgAssert(resAddr.address.octets == dci::utils::ip::masked(ip4Endpoint.address.octets, prefix->PrefixLength));
                                    break;

                                case 2:
                                    resAddr.broadcast.octets = dci::utils::ip::masked(ip4Endpoint.address.octets, prefix->PrefixLength);
                                    break;
                                }
                                ++cnt;
                            }

                            prefix = prefix->Next;
                        }

                        res.push_back(resAddr);
                    }
                }

                addr = addr->Next;
            }

            return res;
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        List<api::link::Ip6Address> ip6For(uint32 ifId)
        {
            List<api::link::Ip6Address> res;

            if(!ensureAddressesFetched() || _addressesBuf.empty())
            {
                return res;
            }

            PIP_ADAPTER_ADDRESSES addr = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(&_addressesBuf[0]);
            while(addr)
            {
                if(ifId == addr->Ipv6IfIndex && IP_ADAPTER_IPV6_ENABLED & addr->Flags)
                {
                    api::link::Ip6Address resAddr{};
                    bool addressValid{};

                    PIP_ADAPTER_UNICAST_ADDRESS uaddr = addr->FirstUnicastAddress;
                    while(uaddr)
                    {
                        if (IP_DAD_STATE::IpDadStatePreferred == uaddr->DadState && AF_INET6 == uaddr->Address.lpSockaddr->sa_family && 128 >= uaddr->OnLinkPrefixLength)
                        {
                            api::Ip6Endpoint ep;
                            addressValid = utils::sockaddr::convert(uaddr->Address.lpSockaddr, uaddr->Address.iSockaddrLength, ep);
                            if(addressValid)
                            {
                                resAddr.address = ep.address;
                                resAddr.prefixLength = uaddr->OnLinkPrefixLength;
                                break;
                            }
                        }
                        uaddr = uaddr->Next;
                    }

                    if(addressValid)
                    {
                        if(addr->Ipv6IfIndex == addr->ZoneIndices[ScopeLevelGlobal])
                        {
                            resAddr.scope = api::link::Ip6AddressScope::global;
                        }
                        else if(addr->Ipv6IfIndex == addr->ZoneIndices[ScopeLevelOrganization])
                        {
                            resAddr.scope = api::link::Ip6AddressScope::site;
                        }
                        else if(addr->Ipv6IfIndex == addr->ZoneIndices[ScopeLevelSite])
                        {
                            resAddr.scope = api::link::Ip6AddressScope::site;
                        }
                        else if(addr->Ipv6IfIndex == addr->ZoneIndices[ScopeLevelAdmin])
                        {
                            resAddr.scope = api::link::Ip6AddressScope::site;
                        }
                        else if(addr->Ipv6IfIndex == addr->ZoneIndices[ScopeLevelLink])
                        {
                            resAddr.scope = api::link::Ip6AddressScope::link;
                        }
                        else if(addr->Ipv6IfIndex == addr->ZoneIndices[ScopeLevelInterface])
                        {
                            resAddr.scope = api::link::Ip6AddressScope::host;
                        }
                        else
                        {
                            resAddr.scope = api::link::Ip6AddressScope::null;
                        }

                        res.push_back(resAddr);
                    }
                }

                addr = addr->Next;
            }

            return res;
        }

    private:
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        bool ensureAddressesFetched()
        {
            if(AddressesFetchResult::null == _addressesFetchResult)
            {
                _addressesBuf.resize(32768);
                PIP_ADAPTER_ADDRESSES addresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(&_addressesBuf[0]);
                ULONG addressesSize{static_cast<ULONG>(_addressesBuf.size())};
                ULONG res = GetAdaptersAddresses(
                            AF_UNSPEC,
                            GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER,
                            NULL,
                            addresses,
                            &addressesSize);

                while(ERROR_BUFFER_OVERFLOW == res)
                {
                    _addressesBuf.resize(addressesSize);
                    addresses = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(&_addressesBuf[0]);
                    addressesSize = static_cast<ULONG>(_addressesBuf.size());

                    res = GetAdaptersAddresses(
                                AF_UNSPEC,
                                GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER,
                                NULL,
                                addresses,
                                &addressesSize);
                }

                switch(res)
                {
                case ERROR_NO_DATA:
                    _addressesBuf.clear();
                    _addressesFetchResult = AddressesFetchResult::ok;
                    break;

                case ERROR_SUCCESS:
                    _addressesFetchResult = AddressesFetchResult::ok;
                    break;

                default:
                    LOGE("GetAdaptersAddresses failed: " << dci::utils::win32::error::last());
                    _addressesBuf.clear();
                    _addressesFetchResult = AddressesFetchResult::fail;
                    break;
                }
            }

            return AddressesFetchResult::ok == _addressesFetchResult;
        }

    private:
        Links* _links;
        enum class AddressesFetchResult
        {
            null,
            ok,
            fail,
        } _addressesFetchResult{};
        std::vector<char> _addressesBuf;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Netioapi::onLinkChange(const LinkChange& change, std::shared_ptr<LinkChangeCommon>& cmn)
    {
        if(!cmn)
        {
            cmn = std::make_shared<LinkChangeCommon>(_links);
        }

        Link* link;
        bool isNew{};
        switch(change._ntype)
        {
        case MibParameterNotification:
        case MibAddInstance:
            link = _links->getLink(change._linkId);
            if(!link)
            {
                isNew = true;
                link = _links->allocLink(change._linkId);
            }
            break;

        case MibDeleteInstance:
            _links->delLink(change._linkId);
            return;

        default:
        case MibInitialNotification:
            return;
        }

        MIB_IF_ROW2 fullRow2{};
        fullRow2.InterfaceIndex = change._linkId;
        fullRow2.InterfaceLuid = change._linkLuid;
        if(DWORD res = GetIfEntry2(&fullRow2))
        {
            LOGE("GetIfEntry2 failed: " << dci::utils::win32::error::make(res));
            _links->delLink(change._linkId);
            return;
        }

        dbgAssert(link);

        link->setHwAddress(List<uint8>{&fullRow2.PhysicalAddress[0], &fullRow2.PhysicalAddress[0] + fullRow2.PhysicalAddressLength});
        link->setName(dci::utils::str::wideConvert(&fullRow2.Alias[0]));
        link->setMtu(fullRow2.Mtu);

        api::link::Flags flags{};
        switch(fullRow2.OperStatus)
        {
        case IfOperStatusUp:
            flags |= api::link::Flags::up;

            if(!fullRow2.InterfaceAndOperStatusFlags.Paused &&
               !fullRow2.InterfaceAndOperStatusFlags.NotMediaConnected &&
               !fullRow2.InterfaceAndOperStatusFlags.NotAuthenticated)
            {
                flags |= api::link::Flags::running;
            }
            break;
        case IfOperStatusDown:
        case IfOperStatusTesting:
        case IfOperStatusUnknown:
        case IfOperStatusDormant:
        case IfOperStatusNotPresent:
        case IfOperStatusLowerLayerDown:
        default:
            break;
        }

        switch(fullRow2.AccessType)
        {
        case NET_IF_ACCESS_LOOPBACK:
            flags |= api::link::Flags::loopback;
            break;
        case NET_IF_ACCESS_BROADCAST:
            flags |= api::link::Flags::broadcast;
            flags |= api::link::Flags::multicast;
            break;
        case NET_IF_ACCESS_POINT_TO_POINT:
            flags |= api::link::Flags::p2p;
            break;
        case NET_IF_ACCESS_POINT_TO_MULTI_POINT:
        default:
            break;
        }
        link->setFlags(flags);

        link->setIp4(cmn->ip4For(change._linkId));
        link->setIp6(cmn->ip6For(change._linkId));

        if(isNew)
        {
            _links->allocatedLinkInitialized(change._linkId, link);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Netioapi::onRouteChange(const RouteChange& change)
    {
        auto processOne = [&]<class Entry, class Endpoint>(socklen_t saddrSize)
        {
            Entry entry;
            Endpoint endpoint;
            if(!utils::sockaddr::convert(reinterpret_cast<const sockaddr*>(&change._row.DestinationPrefix.Prefix), saddrSize, endpoint))
            {
                return;
            }

            entry.dst = endpoint.address;
            entry.dstPrefixLength = change._row.DestinationPrefix.PrefixLength;

            if(!utils::sockaddr::convert(reinterpret_cast<const sockaddr*>(&change._row.NextHop), saddrSize, endpoint))
            {
                return ;
            }
            entry.nextHop = endpoint.address;
            entry.linkId = change._row.InterfaceIndex;

            switch(change._ntype)
            {
            case MibParameterNotification:
            case MibAddInstance:
                _routes->add(entry);
                break;
            default:
                _routes->del(entry);
                break;
            }
        };

        switch(change._row.DestinationPrefix.Prefix.si_family)
        {
        case AF_INET:
            processOne.operator()<api::route::Entry4, api::Ip4Endpoint>(sizeof(change._row.DestinationPrefix.Prefix.Ipv4));
            break;

        case AF_INET6:
            processOne.operator()<api::route::Entry6, api::Ip6Endpoint>(sizeof(change._row.DestinationPrefix.Prefix.Ipv6));
            break;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Netioapi::flushChanges()
    {
        std::deque<LinkChange> linkChanges;
        std::deque<RouteChange> routeChanges;

        {
            std::scoped_lock lock{_mtx};
            linkChanges.swap(_linkChanges);
            routeChanges.swap(_routeChanges);
        }

        if(!linkChanges.empty())
        {
            std::shared_ptr<LinkChangeCommon> cmn;
            for(LinkChange& change : linkChanges)
            {
                onLinkChange(change, cmn);
            }
        }

        if(!routeChanges.empty())
        {
            for(RouteChange& change : routeChanges)
            {
                onRouteChange(change);
            }
            _routes->flushChanges(true);
        }
    }
}
