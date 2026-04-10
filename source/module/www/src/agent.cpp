/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "agent.hpp"
#include "agent/io.hpp"

namespace dci::module::www
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class I>
    cmt::Future<I> Agent::getDependency()
    {
        return cmt::spawnv<I>() += _tol * [this]
        {
            Set<api::agent::DependenciesFactory<>::Opposite> probed;

            for(;;)
            {
                for(const api::agent::DependenciesFactory<>::Opposite& dependenciesFactory : _dependenciesFactories)
                {
                    if(!probed.emplace(dependenciesFactory).second)
                        continue;

                    try
                    {
                        I res = *dependenciesFactory->activated(I::lid());
                        if(res)
                            return res;
                    }
                    catch(...)
                    {
                        LOGW("dependenciesFactory activation failed for " << I::lid().toIidText() << ": " << exception::toString(std::current_exception()));
                    }
                    break;
                }
            }

            return *_hostManager->createService<I>();
        };
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Agent::Agent(host::Manager* hostManager)
        : api::Agent<>::Opposite{idl::interface::Initializer{}}
        , _hostManager{hostManager}
    {
        // in installDependenciesFactory(DependenciesFactory::Opposite);
        methods()->installDependenciesFactory() += serviceSol() * [this](api::agent::DependenciesFactory<>::Opposite&& dependenciesFactory)
        {
            _dependenciesFactories.emplace(std::move(dependenciesFactory));
        };

        // in enableLog() -> agent::log::Source;
        methods()->enableLog() += serviceSol() * [this]()
        {
            if(!_logSink)
                _logSink.init();
            return cmt::readyFuture(_logSink.opposite());
        };

        // in disableLog();
        methods()->disableLog() += serviceSol() * [this]()
        {
            _logSink.reset();
        };

        // in tlsSetupTrust() -> tls::setup::Trust;
        methods()->tlsSetupTrust() += serviceSol() * [this]()
        {
            return _wwwTlsSetupTrust.future();
        };

        // in enableCookies() -> www::http::client::cookies::Store;
        methods()->enableCookies() += serviceSol() * [this]()
        {
            if(_cookies)
                return cmt::readyFuture<api::http::client::cookies::Store<>>(_cookies);

            return getDependency<api::http::client::Cookies<>>().chain() += serviceSol() * [this](cmt::Future<api::http::client::Cookies<>> in, cmt::Promise<api::http::client::cookies::Store<>> out)
            {
                if(in.resolvedException())
                {
                    _fail = exception::buildInstance<api::agent::HostFailed>(in.detachException());
                    out.resolveException(in.detachException());
                }
                else if(in.resolvedCancel())
                {
                    _fail = exception::buildInstance<api::agent::HostFailed>(in.detachException());
                    out.resolveCancel();
                }
                else //if(in.resolvedValue())
                {
                    dbgAssert(in.resolvedValue());
                    _cookies = in.detachValue();
                    out.resolveValue(_cookies);
                }
            };
        };

        // in enableCookiesInside();
        methods()->enableCookiesInside() += serviceSol() * [this]()
        {
            if(_cookies)
                return;

            getDependency<api::http::client::Cookies<>>().then() += serviceSol() * [this](cmt::Future<api::http::client::Cookies<>> in)
            {
                if(in.resolvedException())
                    _fail = exception::buildInstance<api::agent::HostFailed>(in.detachException());
                else if(in.resolvedCancel())
                    _fail = exception::buildInstance<api::agent::HostFailed>(in.detachException());
                else //if(in.resolvedValue())
                    _cookies = in.detachValue();
            };
        };

        // in disableCookies();
        methods()->disableCookies() += serviceSol() * [this]()
        {
            _cookies.reset();
        };

        // in setConncurrency(
        //     uint32 connectionsMax,
        //     uint32 siteConnectionsMax,
        //     uint32 conectionIoPerformingMax,
        //     uint32 siteIoPerformingMax);
        methods()->setConncurrency() += serviceSol() * [this](
                                        uint32 maxConnectionsPerSite,
                                        uint32 idleConnectionTimeoutMs,
                                        uint32 maxIoPerformingPerConection,
                                        uint32 maxIoPerformingPerSite)
        {
            _maxConnectionsPerSite          = maxConnectionsPerSite;
            _idleConnectionTimeoutMs        = idleConnectionTimeoutMs;
            _maxIoPerformingPerConection    = maxIoPerformingPerConection;
            _maxIoPerformingPerSite         = maxIoPerformingPerSite;
        };

        // in hookNet(agent::Hook);
        methods()->hookNet() += serviceSol() * [this](api::agent::Hook<>::Opposite&& hook)
        {
            _hooksNet.emplace_back(std::move(hook));
        };

        // in hookHttp(agent::Hook);
        methods()->hookHttp() += serviceSol() * [this](api::agent::Hook<>::Opposite&& hook)
        {
            _hooksHttp.emplace_back(std::move(hook));
        };

        // in io(agent::Request) -> agent::Response;
        methods()->io() += serviceSol() * [this](api::agent::io::Request&& request)
        {
            return io(std::move(request));
        };

        // in get(string uri, list<www::http::Header> extraHeaders) -> agent::Response;
        methods()->get() += serviceSol() * [this](String&& uri, List<api::http::Header>&& extraHeaders)
        {
            return io(api::agent::io::Request{std::move(uri), api::http::firstLine::Method::GET, std::move(extraHeaders), Bytes{}});
        };

        // in post(string uri, list<www::http::Header> extraHeaders, bytes data) -> agent::Response;
        methods()->post() += serviceSol() * [this](String&& uri, List<api::http::Header>&& extraHeaders, Bytes&& data)
        {
            return io(api::agent::io::Request{std::move(uri), api::http::firstLine::Method::POST, std::move(extraHeaders), std::move(data)});
        };

        cmt::spawn() += _tol * [this]
        {
            try
            {
                _netHost = getDependency<net::Host<>>().value();
                _netStreamClient = _netHost->streamClient().value();
                _netStreamClient->setOption(net::option::Keepalive{true, 10, 5, 3}).value();
                _wwwFactory = getDependency<api::Factory<>>().value();
                _wwwTls = _wwwFactory->tls().value();
                _wwwTls->setAlpnProtos(List<String>{"http/1.1"}).value();
                // _wwwTls->setDefaultTrustedCAs().value();
                // _wwwTls->setVerify(true, 100).value();
                if(!_wwwTlsSetupTrust.resolved())
                    _wwwTlsSetupTrust.resolveValue(_wwwTls);

                auto onInvolvedChanged = [this](bool v)
                {
                    if(!v && !_fail)
                        fail(exception::buildInstance<api::agent::Stopped>());
                };

                _netHost.involvedChanged() += serviceSol() * onInvolvedChanged;
                _netStreamClient.involvedChanged() += serviceSol() * onInvolvedChanged;
                _wwwFactory.involvedChanged() += serviceSol() * onInvolvedChanged;
                _wwwTls.involvedChanged() += serviceSol() * onInvolvedChanged;

                _ready = true;

                dbgAssert(_staff.holds<Ios>());
                Ios ios{std::move(_staff.get<Ios>())};
                _staff.emplace<Sites>();
                ios.release([this](agent::Io* io)
                {
                    io2Site(io);
                });
            }
            catch(...)
            {
                fail(exception::buildInstance<api::agent::HostFailed>(std::current_exception()));
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Agent::~Agent()
    {
        serviceSol().flush();

        _staff.sget<Ios>().clear();

        _netHost.reset();
        _netStreamClient.reset();
        _wwwFactory.reset();
        _wwwTls.reset();
        _wwwTlsSetupTrust.uncharge();
        _cookies.reset();
        _logSink.reset();

        _tol.stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<api::agent::io::Response> Agent::io(api::agent::io::Request&& request)
    {
        switch(request.method)
        {
        case api::http::firstLine::Method::OPTIONS:
        case api::http::firstLine::Method::GET:
        case api::http::firstLine::Method::HEAD:
        case api::http::firstLine::Method::TRACE:
            if(!request.body.empty())
                return cmt::readyFuture<api::agent::io::Response>(exception::buildInstance<api::agent::UnexpectedBody>());
            break;

        case api::http::firstLine::Method::DELETE:
        case api::http::firstLine::Method::POST:
        case api::http::firstLine::Method::PUT:
        case api::http::firstLine::Method::PATCH:
            break;

        case api::http::firstLine::Method::CONNECT:
        default:
            return cmt::readyFuture<api::agent::io::Response>(exception::buildInstance<api::agent::BadMethod>());
        }

        agent::Io* io = new agent::Io{_cookies, std::move(request)};
        cmt::Future<api::agent::io::Response> future = io->future();

        if(_staff.holds<Ios>())
        {
            io->setAgent(this);
            _staff.get<Ios>().push(io);
        }
        else
            io2Site(io);

        return future;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const api::agent::log::Source<>::Opposite& Agent::logSink() const
    {
        return _logSink;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Agent::siteDone(agent::Site* site)
    {
        dbgAssert(_staff.holds<Sites>());
        Sites& sites = _staff.get<Sites>();
        dbgAssert(sites.contains(site->endpoint()) && &(sites.find(site->endpoint()).get_node()->value()) == site);
        sites.erase(sites.iterator_to(*site));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Agent::ioCancelled(agent::Io* io)
    {
        dbgAssert(_staff.holds<Ios>());
        dbgAssert(_staff.get<Ios>().contains(io));

        _staff.get<Ios>().erase(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Agent::ioFailed(agent::Io* io)
    {
        dbgAssert(_staff.holds<Ios>());
        dbgAssert(_staff.get<Ios>().contains(io));

        _staff.get<Ios>().erase(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Agent::fail(ExceptionPtr&& cause)
    {
        if(!_fail)
            _fail = exception::buildInstance<api::agent::HostFailed>(std::move(cause));

        if(_staff.holds<Ios>())
            _staff.get<Ios>().erase([this](agent::Io* io){ io->fail(_fail); });
        else
        {
            Sites& sites = _staff.get<Sites>();
            while(!sites.empty())
                const_cast<agent::Site&>(*sites.begin()).fail(_fail);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Agent::io2Site(agent::Io* io)
    {
        dbgAssert(_staff.holds<Sites>());

        if(io->done())
        {
            delete io;
            return;
        }

        std::expected<agent::site::Endpoint, ExceptionPtr> siteEndpoint = io->calculateSiteEndpoint();
        if(!siteEndpoint.has_value())
        {
            delete io;
            return;
        }

        Sites& sites = _staff.get<Sites>();
        auto iter = sites.lower_bound(siteEndpoint.value());
        if(sites.end() == iter || siteEndpoint.value() != iter->endpoint())
            iter = sites.emplace_hint(iter, this, std::move(siteEndpoint.value()));
        agent::Site& site = const_cast<agent::Site&>(*iter);
        site.perform(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Tuple<api::stream::Channel<> /*local*/, api::stream::Channel<>::Opposite /*remote*/> Agent::makeHookChannelsNet(sbs::Owner& sol)
    {
        return makeHookChannels(sol, _hooksNet);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Tuple<api::stream::Channel<> /*local*/, api::stream::Channel<>::Opposite /*remote*/> Agent::makeHookChannelsHttp(sbs::Owner& sol)
    {
        return makeHookChannels(sol, _hooksHttp);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Tuple<api::stream::Channel<> /*local*/, api::stream::Channel<>::Opposite /*remote*/> Agent::makeHookChannels(sbs::Owner& sol, const List<api::agent::Hook<>::Opposite>& hooks)
    {
        api::stream::Channel<> local;
        api::stream::Channel<>::Opposite remote;

        if(!hooks.empty())
        {
            std::tie(local, remote) = hooks.front()->activated().value();

            for(std::size_t i{1}; i<hooks.size(); ++i)
            {
                auto [hookLocal, hookRemote] = hooks[i]->activated().value();
                Agent::interconnect(hookRemote, sol, local);
                local = hookLocal;
            }
        }

        return {std::move(local), std::move(remote)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Agent::interconnect(const api::stream::Channel<>::Opposite& remote, sbs::Owner& sol, const api::stream::Channel<>& local)
    {
        ////////////////////////////////////////////////////////////////
        remote->close() += sol * [local]()
        {
            local->close();
        };

        remote->send() += sol * [local](auto&& data)
        {
            local->send(std::forward<decltype(data)>(data));
        };

        remote->startReceive() += sol * [local]()
        {
            local->startReceive();
        };

        remote->stopReceive() += sol * [local]()
        {
            local->stopReceive();
        };

        remote->shutdown() += sol * [local]()
        {
            local->shutdown();
        };

        ////////////////////////////////////////////////////////////////
        local->closed() += sol * [remote]()
        {
            remote->closed();
        };

        local->failed() += sol * [remote](auto&& err)
        {
            remote->failed(std::forward<decltype(err)>(err));
        };

        local->received() += sol * [remote](auto&& data)
        {
            remote->received(std::forward<decltype(data)>(data));
        };
    }

}
