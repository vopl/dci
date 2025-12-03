// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
            auto onInvolvedChanged = [this](bool v)
            {
                if(!v && !_fail)
                    fail(exception::buildInstance<api::agent::Stopped>());
            };

            try
            {
                // resolve
                if(_logStream)
                    _logStream->content(String{"try to resolve "} + _site->endpoint()._host);

                List<net::IpEndpoint> ipCandidates;
                try
                {
                    ipCandidates = _agent->_netHost->resolveAllIp(_site->endpoint()._host).value();
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
                }

                if(ipCandidates.empty())
                {
                    if(_logStream)
                        _logStream->content(String{"no candidates to connect"});
                    throw api::agent::ResolveFailed{};
                }

                // connect
                ExceptionPtr lastConnectFail;
                for(net::IpEndpoint& ipCandidate : ipCandidates)
                {
                    ipCandidate.visit([this](auto& ep){ep.port = _site->endpoint()._port;});

                    try
                    {
                        _netChannel = _agent->_netStreamClient->connect(ipCandidate.visit([&](auto&&v){return net::Endpoint{std::move(v)};})).value();

                        _netChannel.involvedChanged() += _sol * onInvolvedChanged;
                        if(_logStream)
                            _logStream->content("connected " + toString(ipCandidate));
                    }
                    catch(...)
                    {
                        lastConnectFail = std::current_exception();
                        if(_logStream)
                            _logStream->content("connect " + toString(ipCandidate) + " failed: " + exception::toString(lastConnectFail));
                        continue;
                    }

                    try
                    {
                        _netChannel->setOption(net::option::Keepalive{true, 10, 5, 4}).value();
                        _netChannel->setOption(net::option::UserTimeout{30*1000}).value();
                    }
                    catch(...)
                    {
                        lastConnectFail = std::current_exception();
                        if(_logStream)
                            _logStream->content("setup socket for " + toString(ipCandidate) + " failed: " + exception::toString(lastConnectFail));
                        _netChannel->shutdown(true, true);
                        _netChannel->close();
                        _netChannel.reset();
                        continue;
                    }

                    break;
                }

                if(!_netChannel)
                {
                    if(_logStream)
                        _logStream->content(String{"no more candidates to connect"});

                    std::rethrow_exception(exception::buildInstance<api::agent::ConnectFailed>(lastConnectFail));
                }

                // forward to www channel
                api::stream::Channel<> nextWwwChannel;
                {
                    api::stream::Channel<>::Opposite nextWwwChannelOpposite = nextWwwChannel.init2();

                    _netChannel->received() += _sol * [nextWwwChannelOpposite](auto&& data)
                    {
                        nextWwwChannelOpposite->received(std::forward<decltype(data)>(data));
                    };

                    _netChannel->failed() += _sol * [this, nextWwwChannelOpposite](auto&& error)
                    {
                        if(_tlsChannel)
                            nextWwwChannelOpposite->failed(std::forward<decltype(error)>(error));
                        else
                        {
                            auto next = [this, error=std::forward<decltype(error)>(error)]
                            {
                                fail(exception::buildInstance<api::agent::NetChannelFailed>(error));
                            };

                            if(connection::State::pending == _state)
                                cmt::spawn() += _tol * next;
                            else
                                next();
                        }
                    };
                    _netChannel->closed() += _sol * [this, nextWwwChannelOpposite]()
                    {
                        if(_logStream)
                            _logStream->content("net closed");

                        if(_tlsChannel)
                            nextWwwChannelOpposite->closed();
                        else
                        {
                            auto next = [this]
                            {
                                close();
                                _site->connectionChanged(this);
                            };

                            if(connection::State::pending == _state)
                                cmt::spawn() += _tol * next;
                            else
                                next();
                        }
                    };

                    nextWwwChannelOpposite->close() += _sol * [this]()
                    {
                        _netChannel->close();
                    };

                    nextWwwChannelOpposite->send() += _sol * [this](auto&& data)
                    {
                        _netChannel->send(std::forward<decltype(data)>(data));
                    };

                    nextWwwChannelOpposite->startReceive() += _sol * [this]()
                    {
                        _netChannel->startReceive();
                    };

                    nextWwwChannelOpposite->stopReceive() += _sol * [this]()
                    {
                        _netChannel->stopReceive();
                    };

                    nextWwwChannelOpposite->shutdown() += _sol * [this]()
                    {
                        _netChannel->shutdown(true, true);
                    };
                }

                // secure
                if(_site->endpoint()._secure)
                {
                    String alpnProtoSelected;
                    try
                    {
                        _tlsChannel = _agent->_wwwTls->client(std::move(nextWwwChannel), Opt<String>{_site->_endpoint._host}).value();
                        alpnProtoSelected = _tlsChannel->alpnProtoSelected().value();
                    }
                    catch(...)
                    {
                        std::rethrow_exception(exception::buildInstance<api::agent::SecureFailed>(std::current_exception()));
                    }

                    _tlsChannel.involvedChanged() += _sol * onInvolvedChanged;
                    if(_logStream)
                        _logStream->content("secured" + (alpnProtoSelected.empty() ? String{} : " for " + alpnProtoSelected));
                    nextWwwChannel = _tlsChannel;

                    _tlsChannel->failed() += _sol * [this](auto&& error)
                    {
                        fail(exception::buildInstance<api::agent::TlsChannelFailed>(std::forward<decltype(error)>(error)));
                    };

                    _tlsChannel->closed() += _sol * [this]()
                    {
                        close();
                        _site->connectionChanged(this);
                    };
                }

                // http
                try
                {
                    _httpChannel = _agent->_wwwFactory->stream2HttpClient(std::move(nextWwwChannel)).value();
                }
                catch(...)
                {
                    std::rethrow_exception(exception::buildInstance<api::agent::HttpFailed>(std::current_exception()));
                }

                _httpChannel.involvedChanged() += _sol * onInvolvedChanged;
                if(_logStream)
                    _logStream->content("http ready");

                _httpChannel->failed() += _sol * [this](auto&& error)
                {
                    fail(exception::buildInstance<api::agent::HttpFailed>(std::forward<decltype(error)>(error)));
                };

                dbgAssert(connection::State::pending == _state);
                _state = connection::State::working;
                _site->connectionChanged(this);
            }
            catch(...)
            {
                fail(std::current_exception());
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connection::~Connection()
    {
        close();
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
        return _iosPerforming.count();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::perform(Io* io)
    {
        switch(_state)
        {
        case connection::State::working:
        case connection::State::full:
            break;
        case connection::State::pending:
        case connection::State::shutdown:
        case connection::State::fail:
        case connection::State::done:
            dbgAssert(false);
            delete io;
            return;
        }

        dbgAssert(_httpChannel);
        dbgAssert(!_fail);

        _iosPerforming.push(io);
        io->setConnection(this);

        if(iosPerformingCount() >= _agent->_maxIoPerformingPerConection)
            _state = connection::State::full;
        else
            _state = connection::State::working;

        io->perform(_httpChannel);

        _site->connectionChanged(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::fail(const ExceptionPtr& fail)
    {
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

        if(_logStream)
            _logStream->content(String{"fail: "} + exception::toString(_fail));

        _iosPerforming.each([this](Io* io){ io->fail(_fail); });

        close();
        _site->connectionChanged(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const ExceptionPtr& Connection::fail() const
    {
        return _fail;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioCancelled(Io* io)
    {
        ioDone(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioFailed(Io* io)
    {
        ioDone(io);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioDone(Io* io)
    {
        dbgAssert(io->started());
        dbgAssert(_iosPerforming.contains(io));
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
            _site->connectionChanged(this);
            break;
        case connection::State::fail:
        case connection::State::done:
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::ioWantClose(Io* io)
    {
        dbgAssert(io->started());
        dbgAssert(_iosPerforming.contains(io));
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
            _site->connectionChanged(this);
            return;
        case connection::State::pending:
        case connection::State::shutdown:
            dbgAssert(false);
            _site->connectionChanged(this);
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
    void Connection::idleLogic(bool forceChangedNotification4Site)
    {
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
                    _logStream->content("idle timeout");
                close();
                _site->connectionChanged(this);
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

        if(forceChangedNotification4Site)
            _site->connectionChanged(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Connection::close()
    {
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

        _iosPerforming.clear();

        if(_logStream)
        {
            _logStream->done();
            _logStream.reset();
        }
    }
}
