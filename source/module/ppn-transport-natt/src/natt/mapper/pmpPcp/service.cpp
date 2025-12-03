// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "service.hpp"
#include "lookout.hpp"
#include "performer.hpp"
#include "msg.hpp"
#include "../../addr.hpp"
#include <experimental/random>

namespace dci::module::ppn::transport::natt::mapper::pmpPcp
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::Service(Lookout* l, const net::IpAddress& clientAddr, const net::Endpoint& srvEp)
        : netBased::Service<Lookout, Service, Performer>{l, clientAddr, srvEp}
    {
        _name = "pmpPcp at " + addr::toString(_srvEp, api::Protocol::udp);

        static_assert(12 == sizeof(_pcpNonce));

        *static_cast<uint64*>(static_cast<void*>(_pcpNonce.data()+0)) = std::experimental::randint(uint64{}, ~uint64{});
        *static_cast<uint32*>(static_cast<void*>(_pcpNonce.data()+8)) = std::experimental::randint(uint32{}, ~uint32{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::~Service()
    {
        _revealTicker.stop();
        _l->deactivate(this);
        _clientSol.flush();
        _client.reset();
        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::mcastReceived(Bytes&& data, const net::Endpoint& ep)
    {
        if(ep != _srvEp)
        {
            return;
        }

        switch(data.size())
        {
        case sizeof(msg::v0::Response_ANNOUNCE):
            {
                if(!_l->usePmp()) return;
                if(data.size() != sizeof(msg::v0::Response_ANNOUNCE)) return;

                msg::v0::Response_ANNOUNCE resp;
                data.begin().read(&resp, sizeof(msg::v0::Response_ANNOUNCE));

                if(!resp._isResponse) return;
                if(msg::v0::ANNOUNCE != resp._opcode) return;
                if(msg::v0::SUCCESS != utils::endian::b2n(resp._resultCode)) return;

                _pmpEpochTime = utils::endian::b2n(resp._epochTime);
                _pmpExternal.octets = resp._externalAddress;
                _pmpState.fixFound(Clock::now());

                if(!_pmpState.ok())
                {
                    LOGI(_name<<": found PMP"<<" by mcast");
                }

                _serviceChanged.raise();
                _pcpState.reset();
                _revealTicker.interval(_timeoutRevealUrgent);
                _revealTicker.start();
            }
            break;

        case sizeof(msg::v1::Response_ANNOUNCE):
            {
                if(!_l->usePcp()) return;
                if(data.size() != sizeof(msg::v1::Response_ANNOUNCE)) return;

                msg::v1::Response_ANNOUNCE resp;
                data.begin().read(&resp, sizeof(msg::v1::Response_ANNOUNCE));

                if(!resp._isResponse) return;
                if(msg::v1::ANNOUNCE != resp._opcode) return;
                if(msg::v1::SUCCESS != resp._resultCode) return;

                _pcpVersion = resp._version;
                _pcpEpochTime = utils::endian::b2n(resp._epochTime);
                _pcpState.fixFound(Clock::now());

                if(!_pcpState.ok())
                {
                    LOGI(_name<<": found PCP v"<<static_cast<int>(_pcpVersion)<<" by mcast");
                }

                _serviceChanged.raise();
                _pmpState.reset();
                _revealTicker.interval(_timeoutRevealUrgent);
                _revealTicker.start();
            }
            break;

        default:
            break;
        }

        updateActivity();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Service::map(net::IpEndpoint& externalEp, uint16 internalPort, api::Protocol p, std::chrono::seconds lifetime)
    {
        if(!_client)
        {
            return false;
        }

        utils::AtScopeExit se{[this]
        {
            updateActivity();
        }};

        if(_l->usePcp() && _pcpState.ok())
        {
            return mapPcp(externalEp, internalPort, p, lifetime);
        }

        if(_l->usePmp() && _pmpState.ok())
        {
            return mapPmp(externalEp, internalPort, p, lifetime);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Service::performerPriority()
    {
        static constexpr int maximal = 20;

        int ban = std::min(_pcpState.ok() ? static_cast<int>(_pcpState._ban) : maximal,
                           _pmpState.ok() ? static_cast<int>(_pmpState._ban) : maximal);

        return std::clamp(maximal-ban, 0, maximal);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::updateActivity()
    {
        if(_client && (_pcpState.ok() || _pmpState.ok()))
        {
            if(_l->activate(this)) _serviceChanged.raise();
        }
        else
        {
            if(_l->deactivate(this)) _serviceChanged.raise();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Service::Timepoint Service::doRevealPayload()
    {
        Timepoint now = Clock::now();

        if(_l->usePcp() && _pcpState.nextReveal() <= now)
        {
            tryPcp();
            now = Clock::now();
        }

        if(_l->usePmp() && _pmpState.nextReveal() <= now)
        {
            tryPmp();
        }

        return std::min(_pmpState.nextReveal(), _pcpState.nextReveal());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::tryPcp()
    {
        auto requestMaker = [this]
        {
            if(_pcpState._lastFound > _pcpState._lastTried) return Bytes{};

            msg::v1::Request_ANNOUNCE req{};

            if(1 == _pcpVersion || 2 == _pcpVersion)    req._version = _pcpVersion;
            else                                        req._version = 2;

            req._opcode = msg::v1::ANNOUNCE;
            fillAddress(req._clientAddress, _clientEp);
            return Bytes{&req, sizeof(req)};
        };

        bool wasOk = _pcpState.ok();
        auto responseHandler = [=,this](Bytes&& data)
        {
            if(data.size() != sizeof(msg::v1::Response_ANNOUNCE)) return InteractResult::null;

            msg::v1::Response_ANNOUNCE resp;
            data.begin().read(&resp, sizeof(msg::v1::Response_ANNOUNCE));

            if(!resp._isResponse) return InteractResult::null;
            if(msg::v1::ANNOUNCE != resp._opcode) return InteractResult::null;

            switch(resp._resultCode)
            {
            case msg::v1::SUCCESS:
                _pcpVersion = resp._version;
                _pcpEpochTime = resp._epochTime;

                if(!wasOk)
                {
                    LOGI(_name<<": found PCP v"<<static_cast<int>(_pcpVersion));
                    _serviceChanged.raise();
                }

                _pcpState.fixFound(Clock::now());
                return InteractResult::success;

            case msg::v1::UNSUPP_VERSION:
                _pcpVersion = resp._version;
                return InteractResult::partial;

            case msg::v1::UNSUPP_OPCODE:
                if(_pcpVersion != resp._version)
                {
                    _pcpVersion = resp._version;
                    return InteractResult::partial;
                }

                _pcpState.fixFailed(Clock::now());
                return InteractResult::fail;

            default:
                LOGI(_name<<": bad ANNOUNCE result code: "<<static_cast<int>(resp._resultCode));
                _pcpState.fixFailed(Clock::now());
                return InteractResult::fail;
            }

            return InteractResult::null;
        };

        _pcpState.fixTried(Clock::now());
        if(InteractResult::success != interact(requestMaker, responseHandler))
        {
            _pcpState.fixFailed(Clock::now());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::tryPmp()
    {
        auto requestMaker = [this]
        {
            if(_pmpState._lastFound > _pmpState._lastTried) return Bytes{};

            msg::v0::Request_ANNOUNCE req{};

            req._version = 0;
            req._opcode = msg::v0::ANNOUNCE;
            return Bytes{&req, sizeof(req)};
        };

        bool wasOk = _pmpState.ok();
        auto responseHandler = [=,this](Bytes&& data)
        {
            if(data.size() != sizeof(msg::v0::Response_ANNOUNCE)) return InteractResult::null;

            msg::v0::Response_ANNOUNCE resp;
            data.begin().read(&resp, sizeof(msg::v0::Response_ANNOUNCE));

            if(!resp._isResponse) return InteractResult::null;
            if(msg::v0::ANNOUNCE != resp._opcode) return InteractResult::null;

            switch(utils::endian::b2n(resp._resultCode))
            {
            case msg::v0::SUCCESS:
                _pmpEpochTime = utils::endian::b2n(resp._epochTime);
                _pmpExternal.octets = resp._externalAddress;

                if(!wasOk)
                {
                    LOGI(_name<<": found PMP");
                    _serviceChanged.raise();
                }

                _pmpState.fixFound(Clock::now());
                return InteractResult::success;

            case msg::v0::UNSUPP_VERSION:
                _pmpState.fixFailed(Clock::now());
                return InteractResult::fail;

            default:
                LOGI(_name<<": bad ANNOUNCE result code: "<<static_cast<int>(resp._resultCode));
                _pcpState.fixFailed(Clock::now());
                return InteractResult::fail;
            }

            return InteractResult::null;
        };

        _pmpState.fixTried(Clock::now());
        if(InteractResult::success != interact(requestMaker, responseHandler))
        {
            _pmpState.fixFailed(Clock::now());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Service::mapPcp(net::IpEndpoint& externalEp, uint16 internalPort, api::Protocol p, std::chrono::seconds lifetime)
    {
        if(1 != _pcpVersion && 2 != _pcpVersion)
        {
            return false;
        }

        auto requestMakerTyped = [&,this](auto& req)
        {
            req._version = _pcpVersion;
            req._opcode = msg::v1::MAP;
            req._lifeTime = static_cast<uint32>(lifetime.count());
            req._protocol = api::Protocol::tcp == p ? 6 : 17;//from the IANA protocol registry, http://www.iana.org/assignments/protocol-numbers

            fillAddress(req._clientAddress, _clientEp);
            req._internalPort = utils::endian::n2b(internalPort);

            if(externalEp.holds<net::Ip4Endpoint>()) req._externalPort = utils::endian::n2b(externalEp.get<net::Ip4Endpoint>().port);
            else                                     req._externalPort = utils::endian::n2b(externalEp.get<net::Ip6Endpoint>().port);

            fillAddress(req._externalAddress, externalEp);
        };

        auto requestMaker = [&,this]
        {
            if(1 == _pcpVersion)
            {
                msg::v1::Request_MAP req{};
                requestMakerTyped(req);
                return Bytes{&req, sizeof(req)};
            }

            if(2 == _pcpVersion)
            {
                msg::v2::Request_MAP req{};
                requestMakerTyped(req);
                req._nonce = _pcpNonce;
                return Bytes{&req, sizeof(req)};
            }

            return Bytes{};
        };

        auto responseHandlerTyped = [&,this](const auto& resp)
        {
            if(resp._version != _pcpVersion) return InteractResult::null;
            if(!resp._isResponse) return InteractResult::null;
            if(msg::v1::MAP != resp._opcode) return InteractResult::null;

            if((resp._protocol == 6  && api::Protocol::tcp != p) ||
               (resp._protocol == 17 && api::Protocol::udp != p))
            {
                return InteractResult::null;
            }

            if(internalPort != utils::endian::b2n(resp._internalPort)) return InteractResult::null;

            if(msg::v1::SUCCESS != resp._resultCode)
            {
                LOGI(_name<<": bad MAP result code: "<<static_cast<int>(resp._resultCode));
                return InteractResult::fail;
            }

            _pcpEpochTime = utils::endian::b2n(resp._epochTime);

            uint32 scope = static_cast<uint32>(utils::ip::scope(resp._externalAddress));

            if(scope & static_cast<uint32>(utils::ip::Scope::ip4))
            {
                net::Ip4Endpoint ep4{{
                        resp._externalAddress[12],
                        resp._externalAddress[13],
                        resp._externalAddress[14],
                        resp._externalAddress[15]}, utils::endian::b2n(resp._externalPort)};

                externalEp = ep4;
            }
            else
            {
                net::Ip6Endpoint ep6{{{resp._externalAddress},0}, utils::endian::b2n(resp._externalPort)};
                externalEp = ep6;
            }

            return InteractResult::success;
        };

        auto responseHandler = [&,this](Bytes&& data)
        {
            if(data.size() == sizeof(msg::v2::Response_MAP))
            {
                msg::v2::Response_MAP resp;
                data.begin().read(&resp, sizeof(msg::v2::Response_MAP));
                if(resp._nonce != _pcpNonce) return InteractResult::null;
                return responseHandlerTyped(resp);
            }

            if(data.size() == sizeof(msg::v1::Response_MAP))
            {
                msg::v1::Response_MAP resp;
                data.begin().read(&resp, sizeof(msg::v1::Response_MAP));
                return responseHandlerTyped(resp);
            }

            if(data.size() == sizeof(msg::v2::Response_MAP))
            {
                msg::v2::Response_MAP resp;
                data.begin().read(&resp, sizeof(msg::v2::Response_MAP));
                return responseHandlerTyped(resp);
            }

            return InteractResult::null;
        };

        if(mapSkeleton(requestMaker, responseHandler, cmt::task::current().stopRequested()))
        {
            return true;
        }

        _pcpState.fixBan();
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Service::mapPmp(net::IpEndpoint& externalEp, uint16 internalPort, api::Protocol p, std::chrono::seconds lifetime)
    {
        auto requestMaker = [&]
        {
            msg::v0::Request_MAP req{};
            req._version = 0;
            req._opcode = api::Protocol::tcp == p ? msg::v0::MAP_TCP : msg::v0::MAP_UDP;

            req._internalPort = utils::endian::n2b(internalPort);

            if(externalEp.holds<net::Ip4Endpoint>()) req._externalPort = utils::endian::n2b(externalEp.get<net::Ip4Endpoint>().port);
            else                                     req._externalPort = utils::endian::n2b(externalEp.get<net::Ip6Endpoint>().port);

            req._lifeTime = static_cast<uint32>(lifetime.count());

            return Bytes{&req, sizeof(req)};
        };

        auto responseHandler = [&,this](Bytes&& data)
        {
            if(data.size() != sizeof(msg::v0::Response_MAP)) return InteractResult::null;

            msg::v0::Response_MAP resp;
            data.begin().read(&resp, sizeof(msg::v0::Response_MAP));

            if(resp._version != 0) return InteractResult::null;
            if(!resp._isResponse) return InteractResult::null;
            if(api::Protocol::tcp == p && msg::v0::MAP_TCP != resp._opcode) return InteractResult::null;
            if(api::Protocol::udp == p && msg::v0::MAP_UDP != resp._opcode) return InteractResult::null;


            if(internalPort != utils::endian::b2n(resp._internalPort)) return InteractResult::null;

            if(msg::v0::SUCCESS != resp._resultCode)
            {
                LOGI(_name<<": bad MAP result code: "<<static_cast<int>(utils::endian::b2n(resp._resultCode)));
                return InteractResult::fail;
            }

            _pmpEpochTime = utils::endian::b2n(resp._epochTime);

            externalEp = net::Ip4Endpoint{_pmpExternal, utils::endian::b2n(resp._externalPort)};

            return InteractResult::success;
        };

        if(mapSkeleton(requestMaker, responseHandler, cmt::task::current().stopRequested()))
        {
            return true;
        }

        _pmpState.fixBan();
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Service::mapSkeleton(auto&& requestMaker, auto&& responseHandler, bool onlySend)
    {
        if(onlySend)
        {
            send(requestMaker);
            return true;
        }

        switch(interact(requestMaker, responseHandler))
        {
        case InteractResult::null:
        case InteractResult::partial:
            if(!_revealTicker.started() || _revealTicker.remaining() > _timeoutRevealUrgent)
            {
                _revealTicker.interval(_timeoutRevealUrgent);
                _revealTicker.start();
            }
            break;

        case InteractResult::fail:
            break;

        case InteractResult::success:
            return true;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::fillAddress(msg::v1::Address& dst, const net::Endpoint& src)
    {
        if(src.holds<net::Ip4Endpoint>())
        {
            dst[10] = 0xff;
            dst[11] = 0xff;
            memcpy(&dst[12], src.get<net::Ip4Endpoint>().address.octets.data(), 4);
        }
        else if(src.holds<net::Ip6Endpoint>())
        {
            memcpy(&dst[0], src.get<net::Ip6Endpoint>().address.octets.data(), 16);
        }
        else
        {
            dst.fill(0);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Service::fillAddress(msg::v1::Address& dst, const net::IpEndpoint& src)
    {
        if(src.holds<net::Ip4Endpoint>())
        {
            dst[10] = 0xff;
            dst[11] = 0xff;
            memcpy(&dst[12], src.get<net::Ip4Endpoint>().address.octets.data(), 4);
        }
        else if(src.holds<net::Ip6Endpoint>())
        {
            memcpy(&dst[0], src.get<net::Ip6Endpoint>().address.octets.data(), 16);
        }
        else
        {
            dst.fill(0);
        }
    }
}
