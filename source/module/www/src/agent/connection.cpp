/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "connection.hpp"
#include "../agent.hpp"
#include "site.hpp"
#include "io.hpp"

namespace dci::module::www::agent
{
    namespace
    {
        std::string toString(const net::IpEndpoint& ipEndpoint)
        {
            return ipEndpoint.visit([&]<class Concrete>(const Concrete& concrete)
            {
                if constexpr(std::is_same_v<Concrete, net::Ip6Endpoint>)
                    return utils::ip::toString(concrete.address.octets, concrete.address.linkId, concrete.port);

                return utils::ip::toString(concrete.address.octets, concrete.port);
            });
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connection::Connection(Agent* agent, Site* site, uint32 id)
        : _agent{agent}
        , _site{site}
        , _id{id}
    {
        if(const api::agent::log::Source<>::Opposite& logSink = _agent->logSink())
        {
            _logStream.init();
            _logStream.involvedChanged() += _sol * [this](bool v)
            {
                if(!v && _logStream)
                {
                    LOGD("logStream uninvolved for " << this->_id);
                    _logStream.reset();
                }
            };

            const auto& host = _site->endpoint()._host;
            bool need5Brackets = host.contains(':') && !host.contains('[');

            std::string streamName = need5Brackets ? ('[' + host + ']') : host;
            streamName += ':' + std::to_string(_site->endpoint()._port);
            streamName += '#' + std::to_string(id);

            logSink->stream(_logStream.opposite(), streamName);
        }
        else
            LOGD("no logSink for " << this->_id);

        cmt::spawn() += _tol * [this]
        {
            auto holder{rcptr()};
            if(!isWorkable())
                return;

            auto onInvolvedChanged = [this](bool v)
            {
                auto holder{rcptr()};
                if(!v && !_fail)
                    fail(exception::buildInstance<api::agent::Stopped>());
            };

            auto emergencyLogging = utils::AtScopeExit{[this]
            {
                if(_logStream)
                    _logStream->content(String{"emergency stop"});
            }};

            try
            {
                // resolve
                if(_logStream)
                {
                    _logStream->content(String{"try to resolve "} + _site->endpoint()._host);
                    if(!isWorkable())
                        return;
                }

                List<net::IpEndpoint> ipCandidates;
                try
                {
                    ipCandidates = _agent->_netHost->resolveAllIp(_site->endpoint()._host).value();
                    if(!isWorkable())
                        return;
                }
                catch(...)
                {
                    std::rethrow_exception(exception::buildInstance<api::agent::ResolveFailed>(std::current_exception()));
                }

                if(_logStream)
                {
                    String ips;
                    for(const net::IpEndpoint& ipCandidate : ipCandidates)
                    {
                        if(!ips.empty())
                            ips += ", ";
                        ips += toString(ipCandidate);
                    }
                    _logStream->content("resolved: " + ips);
                    if(!isWorkable())
                        return;
                }

                if(ipCandidates.empty())
                {
                    if(_logStream)
                    {
                        _logStream->content(String{"no candidates to connect"});
                        if(!isWorkable())
                            return;
                    }
                    throw api::agent::ResolveFailed{};
                }

                // connect
                ExceptionPtr lastConnectFail;
                for(net::IpEndpoint& ipCandidate : ipCandidates)
                {
                    ipCandidate.visit([this](auto& ep){ep.port = _site->endpoint()._port;});

                    auto processCurrentException = [&](std::string prefix)
                    {
                        lastConnectFail = std::current_exception();
                        if(_logStream)
                        {
                            _logStream->content(prefix + " " + toString(ipCandidate) + " failed: " + exception::toString(lastConnectFail));
                            if(!isWorkable())
                                return;
                        }
                    };

                    try
                    {
                        if(_logStream)
                        {
                            _logStream->content(String{"try to connect "} + toString(ipCandidate));
                            if(!isWorkable())
                                return;
                        }

                        _netChannel = _agent->_netStreamClient->connect(ipCandidate.visit([&](auto&&v){return net::Endpoint{std::move(v)};})).value();
                        if(!isWorkable())
                            return;

                        _netChannel.involvedChanged() += _sol * onInvolvedChanged;
                        if(_logStream)
                        {
                            _logStream->content("connected " + toString(ipCandidate));
                            if(!isWorkable())
                                return;
                        }
                    }
                    catch(const cmt::task::Stop&)
                    {
                        processCurrentException("connect");
                        return;
                    }
                    catch(...)
                    {
                        processCurrentException("connect");
                        continue;
                    }

                    try
                    {
                        _netChannel->setOption(net::option::Keepalive{true, 10, 5, 4}).value();
                        if(!isWorkable())
                            return;
                        _netChannel->setOption(net::option::UserTimeout{30*1000}).value();
                        if(!isWorkable())
                            return;
                    }
                    catch(const cmt::task::Stop&)
                    {
                        processCurrentException("setup socket for");
                        return;
                    }
                    catch(...)
                    {
                        processCurrentException("setup socket for");
                        _netChannel->shutdown(true, true);
                        if(!isWorkable())
                            return;
                        _netChannel->close();
                        if(!isWorkable())
                            return;
                        _netChannel.reset();
                        if(!isWorkable())
                            return;
                        continue;
                    }

                    break;
                }

                if(!_netChannel)
                {
                    if(_logStream)
                    {
                        _logStream->content(String{"no more candidates to connect"});
                        if(!isWorkable())
                            return;
                    }

                    std::rethrow_exception(exception::buildInstance<api::agent::ConnectFailed>(lastConnectFail));
                }

                // forward to www channel
                api::stream::Channel<> local;
                {
                    api::stream::Channel<>::Opposite remote;
                    std::tie(local, remote) = _agent->makeHookChannelsNet(_sol);
                    if(!isWorkable())
                        return;

                    dbgAssert(!local == !remote);
                    if(!local)
                    {
                        remote = local.init2();
                    }

                    _netChannel->received() += _sol * [remote](auto&& data)
                    {
                        remote->received(std::forward<decltype(data)>(data));
                    };

                    _netChannel->failed() += _sol * [this, remote](auto&& error)
                    {
                        auto holder{rcptr()};

                        if(_tlsChannel)
                            remote->failed(std::forward<decltype(error)>(error));
                        else
                        {
                            auto next = [this, error=std::forward<decltype(error)>(error)]
                            {
                                auto holder{rcptr()};
                                fail(exception::buildInstance<api::agent::NetChannelFailed>(error));
                            };

                            if(connection::State::pending == _state)
                                cmt::spawn() += _tol * next;
                            else
                                next();
                        }
                    };
                    _netChannel->closed() += _sol * [this, remote]()
                    {
                        auto holder{rcptr()};

                        if(_logStream)
                        {
                            _logStream->content("net closed");
                            if(!isWorkable())
                                return;
                        }

                        if(_tlsChannel)
                        {
                            remote->closed();
                            if(!isWorkable())
                                return;
                        }
                        else
                        {
                            auto next = [this]
                            {
                                auto holder{rcptr()};
                                close();
                            };

                            if(connection::State::pending == _state)
                                cmt::spawn() += _tol * next;
                            else
                                next();
                        }
                    };

                    remote->close() += _sol * [this]()
                    {
                        auto holder{rcptr()};
                        _netChannel->close();
                    };

                    remote->send() += _sol * [this](auto&& data)
                    {
                        auto holder{rcptr()};
                        _netChannel->send(std::forward<decltype(data)>(data));
                    };

                    remote->startReceive() += _sol * [this]()
                    {
                        auto holder{rcptr()};
                        _netChannel->startReceive();
                    };

                    remote->stopReceive() += _sol * [this]()
                    {
                        auto holder{rcptr()};
                        _netChannel->stopReceive();
                    };

                    remote->shutdown() += _sol * [this]()
                    {
                        auto holder{rcptr()};
                        _netChannel->shutdown(true, true);
                    };
                }

                // secure
                if(_site->endpoint()._secure)
                {
                    String alpnProtoSelected;
                    try
                    {
                        _tlsChannel = _agent->_wwwTls->client(std::move(local), Opt<String>{_site->_endpoint._host}).value();
                        if(!isWorkable())
                            return;
                        alpnProtoSelected = _tlsChannel->alpnProtoSelected().value();
                        if(!isWorkable())
                            return;
                    }
                    catch(...)
                    {
                        std::rethrow_exception(exception::buildInstance<api::agent::SecureFailed>(std::current_exception()));
                    }

                    _tlsChannel.involvedChanged() += _sol * onInvolvedChanged;
                    if(_logStream)
                    {
                        _logStream->content("secured" + (alpnProtoSelected.empty() ? String{} : " for " + alpnProtoSelected));
                        if(!isWorkable())
                            return;
                    }
                    local = _tlsChannel;

                    _tlsChannel->failed() += _sol * [this](auto&& error)
                    {
                        auto holder{rcptr()};
                        fail(exception::buildInstance<api::agent::TlsChannelFailed>(std::forward<decltype(error)>(error)));
                    };

                    _tlsChannel->closed() += _sol * [this]()
                    {
                        auto holder{rcptr()};
                        close();
                    };
                }

                if(auto [hookLocal, hookRemote] = _agent->makeHookChannelsHttp(_sol); hookLocal)
                {
                    Agent::interconnect(hookRemote, _sol, local);
                    local = hookLocal;
                }

                // http
                try
                {
                    _httpChannel = _agent->_wwwFactory->stream2HttpClient(std::move(local)).value();
                    if(!isWorkable())
                        return;
                }
                catch(...)
                {
                    std::rethrow_exception(exception::buildInstance<api::agent::HttpFailed>(std::current_exception()));
                }

                _httpChannel.involvedChanged() += _sol * onInvolvedChanged;
                if(_logStream)
                {
                    _logStream->content("http ready");
                    if(!isWorkable())
                        return;
                }

                _httpChannel->failed() += _sol * [this](auto&& error)
                {
                    auto holder{rcptr()};
                    fail(exception::buildInstance<api::agent::HttpFailed>(std::forward<decltype(error)>(error)));
                };

                dbgAssert(connection::State::pending == _state);
                _state = connection::State::working;
                if(_site)
                    _site->connectionChanged(holder);
            }
            catch(...)
            {
                fail(std::current_exception());
            }
            emergencyLogging.release();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connection::~Connection()
    {
        close(false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::setAgent(Agent* agent)
    {
        dbgAssert(!agent);
        _agent = agent;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::setSite(Site* site)
    {
        dbgAssert(!site);
        _site = site;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Connection::id() const
    {
        return _id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    connection::State Connection::state() const
    {
        return _state;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Connection::iosPerformingCount() const
    {
        return _iosPerforming.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::perform(RCPtr<Io>&& io)
    {
        auto holder{rcptr()};

        switch(_state)
        {
        case connection::State::working:
        case connection::State::full:
            break;
        case connection::State::pending:
        case connection::State::shutdown:
        case connection::State::done:
            dbgAssert(false);
        case connection::State::fail:
            if(_fail)
                io->fail(_fail);
            else
                io->fail(exception::buildInstance<api::agent::Error>());
            return;
        }

        dbgAssert(_httpChannel);
        dbgAssert(!_fail);

        io->setConnection(this);
        _iosPerforming.insert(io);

        if(iosPerformingCount() >= _agent->_maxIoPerformingPerConection)
            _state = connection::State::full;
        else
            _state = connection::State::working;

        io->perform(_httpChannel);

        if(_site)
            _site->connectionChanged(holder);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::fail(const ExceptionPtr& fail)
    {
        auto holder{rcptr()};

        switch(_state)
        {
        case connection::State::pending:
        case connection::State::working:
        case connection::State::full:
        case connection::State::shutdown:
            break;
        case connection::State::fail:
        case connection::State::done:
            return;
        }

        _state = connection::State::fail;

        if(!_fail)
            _fail = fail;

        {
            auto iosPerforming{_iosPerforming};
            for(const RCPtr<Io>& io : iosPerforming)
                io->fail(fail);
        }

        if(_logStream)
            _logStream->content(String{"fail: "} + exception::toString(_fail));

        close();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const ExceptionPtr& Connection::fail() const
    {
        return _fail;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioCancelled(const RCPtr<Io>& io)
    {
        ioDone(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioFailed(const RCPtr<Io>& io)
    {
        ioDone(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioDone(const RCPtr<Io>& io)
    {
        auto holder{rcptr()};

        dbgAssert(io->started());
        dbgAssert(_iosPerforming.contains(io));
        io->setConnection({});
        _iosPerforming.erase(io);

        switch(_state)
        {
        case connection::State::working:
        case connection::State::full:
            idleLogic(true);
            return;
        case connection::State::pending:
        case connection::State::shutdown:
            dbgAssert(false);
            if(_site)
                _site->connectionChanged(holder);
            break;
        case connection::State::fail:
        case connection::State::done:
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioWantClose(const RCPtr<Io>& io)
    {
        auto holder{rcptr()};

        dbgAssert(io->started());
        dbgAssert(_iosPerforming.contains(io));
        io->setConnection({});
        _iosPerforming.erase(io);

        switch(_state)
        {
        case connection::State::working:
        case connection::State::full:
            if(_httpChannel)
            {
                _state = connection::State::shutdown;
                _httpChannel->close();
            }
            else
                close();
            return;
        case connection::State::pending:
        case connection::State::shutdown:
            dbgAssert(false);
            if(_site)
                _site->connectionChanged(holder);
            break;
        case connection::State::fail:
        case connection::State::done:
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const api::agent::log::Stream<>::Opposite& Connection::logStream()
    {
        return _logStream;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::idleLogic(bool notifySite)
    {
        auto holder{rcptr()};

        bool isIdle{};
        switch(_state)
        {
        case connection::State::working:
        case connection::State::full:
            if(iosPerformingCount() >= _agent->_maxIoPerformingPerConection)
                _state = connection::State::full;
            else
                _state = connection::State::working;

            isIdle = _iosPerforming.empty();
            break;
        case connection::State::pending:
        case connection::State::shutdown:
        case connection::State::fail:
        case connection::State::done:
            return;
        }

        if(isIdle && _agent->_idleConnectionTimeoutMs)
        {
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

            if(!_idleBound.time_since_epoch().count())
            {
                _idleBound = now + std::chrono::milliseconds{_agent->_idleConnectionTimeoutMs};
                _idleTicker.interval(std::chrono::milliseconds{_agent->_idleConnectionTimeoutMs});
                _idleTicker.restart();
            }
            else if(_idleBound > now)
            {
                _idleTicker.interval(_idleBound - now);
                _idleTicker.start();
            }
            else
            {
                if(_logStream)
                {
                    _logStream->content("idle timeout");
                    if(!isWorkable())
                        return;
                }

                close();
                return;
            }
        }
        else
        {
            if(_idleBound.time_since_epoch().count())
            {
                _idleBound = {};
                _idleTicker.stop();
            }
        }

        if(notifySite && _site)
            _site->connectionChanged(holder);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::close(bool notifySite)
    {
        Opt<RCPtr<Connection>> holderOpt;
        if(notifySite)
            holderOpt = rcptr();

        switch(_state)
        {
        case connection::State::pending:
        case connection::State::working:
        case connection::State::full:
        case connection::State::shutdown:
            _state = connection::State::done;
            break;
        case connection::State::fail:
        case connection::State::done:
            break;
        }

        _sol.flush();

        if(_httpChannel)
        {
            _httpChannel->close();
            _httpChannel.reset();
        }

        if(_tlsChannel)
        {
            _tlsChannel->shutdown();
            _tlsChannel->close();
            _tlsChannel.reset();
        }

        if(_netChannel)
        {
            _netChannel->shutdown(true, true);
            _netChannel->close();
            _netChannel.reset();
        }

        {
            auto iosPerforming{std::move(_iosPerforming).extract()};
            for(const RCPtr<Io>& io : iosPerforming)
                io->setConnection({});
        }

        _tol.flush();

        if(_logStream)
        {
            _logStream->done();
            _logStream.reset();
        }

        if(_site && holderOpt)
            _site->connectionChanged(*holderOpt);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Connection::isWorkable() const
    {
        if(!_site || !_agent)
            return false;

        switch(_state)
        {
        case connection::State::shutdown:
        case connection::State::fail:
        case connection::State::done:
            return false;
        default:
            break;
        }

        return !_fail;
    }
}
