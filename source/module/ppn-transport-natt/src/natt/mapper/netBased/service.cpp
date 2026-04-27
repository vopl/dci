/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "service.hpp"
#include "../../addr.hpp"

namespace dci::module::ppn::transport::natt::mapper::netBased
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    void Service<MostLookout, MostService, MostPerformer>::State::fixTried(Timepoint tp)
    {
        _lastTried = tp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    void Service<MostLookout, MostService, MostPerformer>::State::fixFound(Timepoint tp)
    {
        _lastFound = tp;
        _ban = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    void Service<MostLookout, MostService, MostPerformer>::State::fixFailed(Timepoint tp)
    {
        _lastFailed = tp;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    void Service<MostLookout, MostService, MostPerformer>::State::fixBan()
    {
        ++_ban;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    typename Service<MostLookout, MostService, MostPerformer>::Timepoint Service<MostLookout, MostService, MostPerformer>::State::nextReveal() const
    {
        Timepoint last = std::max(_lastTried, _lastFound);

        if(ok())
        {
            return last + _timeoutReveal;
        }

        return last + _timeoutRevealAfterFail;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    bool Service<MostLookout, MostService, MostPerformer>::State::ok() const
    {
        return _lastFound > _lastFailed &&
               _lastFound > _lastTried &&
               _ban < 3;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    void Service<MostLookout, MostService, MostPerformer>::State::reset()
    {
        _lastTried =_lastFound = _lastFailed = {};
        _ban = {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    Service<MostLookout, MostService, MostPerformer>::Service(MostLookout* l, const net::IpAddress& clientAddr, const net::Endpoint& srvEp, std::chrono::milliseconds startAfter)
        : _l{l}
        , _clientAddr{clientAddr}
        , _srvEp{srvEp}
        , _clientEp
        {
              _clientAddr.holds<net::Ip4Address>() ?
                  net::Endpoint{net::Ip4Endpoint{_clientAddr.get<net::Ip4Address>(), 0}} :
                  net::Endpoint{net::Ip6Endpoint{_clientAddr.get<net::Ip6Address>(), 0}}
        }
    {
        _revealTicker.tick() += [this]{static_cast<MostService*>(this)->reveal();};
        _revealTicker.interval(startAfter);
        _revealTicker.start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    Service<MostLookout, MostService, MostPerformer>::~Service()
    {
        _revealTicker.stop();
        _l->deactivate(this);
        _clientSol.flush();
        _client.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    PerformerBlank Service<MostLookout, MostService, MostPerformer>::candidate(const apit::Address& internal)
    {
        api::Protocol protocol;
        net::IpEndpoint internalEp;
        net::IpEndpoint externalEp;
        uint16 internalPort {};

        if(!addr::fromString(internal.value, protocol, internalEp))
        {
            return {};
        }

        if(internalEp.holds<net::Ip4Endpoint>() && _clientAddr.holds<net::Ip4Address>())
        {
            const net::Ip4Endpoint& internalEp4 = internalEp.get<net::Ip4Endpoint>();
            const net::Ip4Endpoint& clientEp4 = _clientEp.get<net::Ip4Endpoint>();

            if(internalEp4.address != clientEp4.address)
            {
                return {};
            }
            internalPort = internalEp4.port;
            externalEp.sget<net::Ip4Endpoint>().port = internalPort;
        }
        else if(internalEp.holds<net::Ip6Endpoint>() && _clientAddr.holds<net::Ip6Address>())
        {
            const net::Ip6Endpoint& internalEp6 = internalEp.get<net::Ip6Endpoint>();
            const net::Ip6Endpoint& clientEp6 = _clientEp.get<net::Ip6Endpoint>();

            if(internalEp6.address != clientEp6.address)
            {
                return {};
            }
            internalPort = internalEp6.port;
            externalEp.sget<net::Ip6Endpoint>().port = internalPort;
        }
        else
        {
            return {};
        }

        return PerformerBlank
        {
            [=,this]
            {
                return PerformerPtr
                {
                    new MostPerformer{static_cast<MostService*>(this), protocol, internalPort, externalEp},
                    [](mapper::Performer*p){delete static_cast<MostPerformer*>(p);}
                };
            },
            static_cast<MostService*>(this)->performerPriority()
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    cmt::Waitable& Service<MostLookout, MostService, MostPerformer>::serviceChangedWaitable()
    {
        return _serviceChanged;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class MostLookout, class MostService, class MostPerformer>
    void Service<MostLookout, MostService, MostPerformer>::reveal()
    {
        if(_revealDepth) return;
        ++_revealDepth;
        utils::AtScopeExit se{[this]
        {
            --_revealDepth;
        }};

        try
        {
            if(!_client)
            {
                if(!_l->netHost())
                {
                    _l->deactivate(this);
                    _revealTicker.interval(_timeoutRevealAnormal);
                    _revealTicker.start();
                    return;
                }

                _client = _l->netHost()->datagramChannel().value();

                _client->bind(_clientEp).value();
                _clientEp = _client->localEndpoint().value();

                _client->failed() += _clientSol * [this](ExceptionPtr&& e)
                {
                    LOGW(_name<<": client failed: "<<exception::toString(std::move(e)));
                    _clientSol.flush();
                    _client.reset();
                };

                _client->closed() += _clientSol * [this]()
                {
                    LOGW(_name<<": client closed");
                    _clientSol.flush();
                    _client.reset();
                };

                _client.involvedChanged() += _clientSol * [this](bool v)
                {
                    if(!v)
                    {
                        LOGW(_name<<": client uninvolved");
                        _clientSol.flush();
                        _client.reset();
                    }
                };
            }

            Timepoint nextReveal = static_cast<MostService*>(this)->doRevealPayload();
            Timepoint now = Clock::now();

            if(nextReveal <= now+_timeoutRevealAmortization)
            {
                _revealTicker.interval(_timeoutRevealAmortization);
            }
            else
            {
                _revealTicker.interval(std::chrono::duration_cast<std::chrono::milliseconds>(nextReveal - now));
            }

            _revealTicker.start();

            static_cast<MostService*>(this)->updateActivity();
        }
        catch(const cmt::task::Stop&)
        {
            _l->deactivate(this);
        }
        catch(...)
        {
            LOGW(_name<<": reveal failed: "<<exception::toString(std::current_exception()));
            _l->deactivate(this);

            _revealTicker.interval(_timeoutRevealAnormal);
            _revealTicker.start();
        }
    }
}

#include "../pmpPcp/lookout.hpp"
#include "../pmpPcp/service.hpp"
#include "../pmpPcp/performer.hpp"
template class dci::module::ppn::transport::natt::mapper::netBased::Service<dci::module::ppn::transport::natt::mapper::pmpPcp::Lookout, dci::module::ppn::transport::natt::mapper::pmpPcp::Service, dci::module::ppn::transport::natt::mapper::pmpPcp::Performer>;

#include "../igdp/lookout.hpp"
#include "../igdp/service.hpp"
#include "../igdp/performer.hpp"
template class dci::module::ppn::transport::natt::mapper::netBased::Service<dci::module::ppn::transport::natt::mapper::igdp::Lookout, dci::module::ppn::transport::natt::mapper::igdp::Service, dci::module::ppn::transport::natt::mapper::igdp::Performer>;

