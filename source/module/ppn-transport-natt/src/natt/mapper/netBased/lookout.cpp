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

namespace dci::module::ppn::transport::natt::mapper::netBased
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    Lookout<MostLookout, MostService>::Lookout(Mapper* mapper)
        : mapper::Lookout{mapper}
    {
        cmt::spawn() += _tol * [this]
        {
            try
            {
                worker();
            }
            catch(const cmt::task::Stop&)
            {
                return;
            }
            catch(...)
            {
                LOGW(exception::toString(std::current_exception()));
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    Lookout<MostLookout, MostService>::~Lookout()
    {
        _mcastListner4._sol.flush();
        _mcastListner4._ch.reset();

        for(auto&[id,lst] : _mcastListners6)
        {
            lst._sol.flush();
        }
        _mcastListners6.clear();

        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    const net::Host<>& Lookout<MostLookout, MostService>::netHost()
    {
        return _netHost;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    cmt::Waitable* Lookout<MostLookout, MostService>::regularTicker()
    {
        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    void Lookout<MostLookout, MostService>::regularTick()
    {
        //ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    void Lookout<MostLookout, MostService>::worker()
    {
        _netHost = mapper()->hostManager()->template createService<net::Host<>>().value();

        _netHost->route4Changed() += _sol * [this]
        {
            _netChanged.raise();
        };
        _netHost->route6Changed() += _sol * [this]
        {
            _netChanged.raise();
        };

        auto setupLink = [this](LinkId id, const net::Link<>& api)
        {
            Link& link = _links[id];
            link._sol.flush();
            link._id = id;
            link._api = api;

            link._api->changed() += link._sol * [this]
            {
                _netChanged.raise();
            };

            link._api->removed() += link._sol * [id, this]
            {
                auto iter = _links.find(id);
                if(_links.end() == iter)
                {
                    return;
                }
                Link& link = iter->second;
                link._sol.flush();
                _links.erase(iter);
                _netChanged.raise();
            };

            link._api.involvedChanged() += link._sol * [id, this](bool v)
            {
                if(!v)
                {
                    auto iter = _links.find(id);
                    if(_links.end() == iter)
                    {
                        return;
                    }
                    Link& link = iter->second;
                    link._sol.flush();
                    _links.erase(iter);
                    _netChanged.raise();
                }
            };
        };

        _netHost->linkAdded() += _sol * [setupLink, this](LinkId id, const net::Link<>& api)
        {
            setupLink(id, api);
            _netChanged.raise();
        };

        auto links = _netHost->links().value();
        for(const auto&[id, api] : links)
        {
            setupLink(id, api);
        }

        for(;;)
        {
            {
                Gateways gateways;
                {
                    List<net::route::Entry4> route4{_netHost->route4().value()};
                    for(const net::route::Entry4& e : route4)
                    {
                        if(e.nextHop != net::Ip4Address{})
                        {
                            gateways.insert(e.nextHop);
                        }
                    }
                }

                {
                    List<net::route::Entry6> route6{_netHost->route6().value()};
                    for(const net::route::Entry6& e : route6)
                    {
                        if(e.nextHop != net::Ip6Address{})
                        {
                            gateways.insert(net::Ip6Address{e.nextHop.octets, e.linkId});
                        }
                    }
                }

                _gateways.swap(gateways);
            }

            {
                LinkAddresses4  linkAddresses4;
                LinkAddresses6  linkAddresses6;

                for(auto&[linkId, link] : _links)
                {
                    auto addr = link._api->addr().value();
                    for(const net::link::Address& la : addr)
                    {
                        if(la.holds<net::link::Ip4Address>()) linkAddresses4.insert(la.get<net::link::Ip4Address>());
                        if(la.holds<net::link::Ip6Address>()) linkAddresses6.insert(la.get<net::link::Ip6Address>());
                    }
                }

                _linkAddresses4.swap(linkAddresses4);
                _linkAddresses6.swap(linkAddresses6);
            }

            static_cast<MostLookout*>(this)->updateMcastListeners();
            static_cast<MostLookout*>(this)->updateServices();

            cmt::Waitable* regularTicker = static_cast<MostLookout*>(this)->regularTicker();

            if(regularTicker)
            {
                for(;;)
                {
                    if(0 == cmt::waitAny(*regularTicker, _netChanged))
                    {
                        static_cast<MostLookout*>(this)->regularTick();
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                _netChanged.wait();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    void Lookout<MostLookout, MostService>::updateMcastListeners()
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

        auto mcastSetupper = [&](McastListner& lst, const auto& ep)
        {
            lst._ch = _netHost->datagramChannel().value();
            lst._ch->setOption(net::option::ReuseAddr{true}).value();

            constexpr bool isWindows = false;
            if(isWindows)   lst._ch->bind(std::decay_t<decltype(ep)>{{}, MostLookout::_mcastListenPort}).value();
            else            lst._ch->bind(ep).value();

            lst._ch->setOption(net::option::JoinMulticast{ep.address}).value();

            lst._ch->received() += lst._sol * [this](Bytes&& data, const net::Endpoint& ep)
            {
                static_cast<MostLookout*>(this)->mcastReceived(std::move(data), ep);
            };

            lst._ch->failed() += lst._sol * [&lst,ep](ExceptionPtr&& e)
            {
                LOGW("mcast listner failed for "<<addr::toString(ep, api::Protocol::udp)<<": "<<exception::toString(std::move(e)));
                lst._sol.flush();
                lst._ch.reset();
            };

            lst._ch->closed() += lst._sol * [&lst,ep]()
            {
                LOGI("mcast listner closed for "<<addr::toString(ep, api::Protocol::udp));
                lst._sol.flush();
                lst._ch.reset();
            };

            lst._ch.involvedChanged() += lst._sol * [&lst,ep](bool v)
            {
                if(!v)
                {
                    LOGW("mcast listner uninvolved for "<<addr::toString(ep, api::Protocol::udp));
                    lst._sol.flush();
                    lst._ch.reset();
                }
            };
        };

        if(needIp4 && !_mcastListner4._ch)
        {
            mcastSetupper(_mcastListner4, net::Ip4Endpoint{{MostLookout::_mcastListenAddr4}, MostLookout::_mcastListenPort});
        }

        if(!needIp4 && _mcastListner4._ch)
        {
            _mcastListner4._ch->close();
            _mcastListner4._ch.reset();
        }

        for(auto iter{_mcastListners6.begin()}; iter!=_mcastListners6.end();)
        {
            if(needIp6LinkIds.contains(iter->first))
            {
                ++iter;
            }
            else
            {
                iter->second._ch->close();
                iter = _mcastListners6.erase(iter);
            }
        }

        for(uint32 linkId : needIp6LinkIds)
        {
            McastListner& lst = _mcastListners6[linkId];
            if(!lst._ch)
            {
                mcastSetupper(lst, net::Ip6Endpoint{{MostLookout::_mcastListenAddr6, linkId}, MostLookout::_mcastListenPort});
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    std::deque<net::IpAddress> Lookout<MostLookout, MostService>::linkAddressesFor(const net::IpAddress& addr)
    {
        std::deque<net::IpAddress> res;

        if(addr.holds<net::Ip4Address>())
        {
            const net::Ip4Address& addr4 = addr.get<net::Ip4Address>();

            for(const net::link::Ip4Address& la4 : _linkAddresses4)
            {
                if(/*addr4 != la4.address &&*/ utils::ip::masked(la4.address.octets, la4.netmask.octets) == utils::ip::masked(addr4.octets, la4.netmask.octets))
                {
                    res.push_back(la4.address);
                }
            }
        }
        else
        {
            const net::Ip6Address& addr6 = addr.get<net::Ip6Address>();

            for(const net::link::Ip6Address& la6 : _linkAddresses6)
            {
                if(/*addr6 != la6.address &&*/ utils::ip::masked(la6.address.octets, la6.prefixLength) == utils::ip::masked(addr6.octets, la6.prefixLength))
                {
                    res.push_back(la6.address);
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService>
    void Lookout<MostLookout, MostService>::updateServices()
    {
        for(const net::IpAddress& gw : _gateways)
        {
            net::Endpoint srvEp;
            if(gw.holds<net::Ip4Address>())
            {
                srvEp = net::Ip4Endpoint{gw.get<net::Ip4Address>(), MostLookout::_servicePort};
            }
            else
            {
                srvEp = net::Ip6Endpoint{gw.get<net::Ip6Address>(), MostLookout::_servicePort};
            }

            for(const net::IpAddress& la : linkAddressesFor(gw))
            {
                _services.emplace(std::piecewise_construct_t{},
                                  std::tie(la, srvEp),
                                  std::forward_as_tuple(static_cast<MostLookout*>(this), la, srvEp));
            }
        }
    }
}

#include "../pmpPcp/lookout.hpp"
#include "../pmpPcp/service.hpp"
template class dci::module::ppn::transport::natt::mapper::netBased::Lookout<dci::module::ppn::transport::natt::mapper::pmpPcp::Lookout, dci::module::ppn::transport::natt::mapper::pmpPcp::Service>;

#include "../igdp/lookout.hpp"
#include "../igdp/service.hpp"
template class dci::module::ppn::transport::natt::mapper::netBased::Lookout<dci::module::ppn::transport::natt::mapper::igdp::Lookout, dci::module::ppn::transport::natt::mapper::igdp::Service>;
