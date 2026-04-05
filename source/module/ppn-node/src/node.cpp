/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "dci/cmt/functions.hpp"
#include "pch.hpp"
#include "node.hpp"
#include "node/utils.hpp"

extern dci::host::module::Entry* dciModuleEntry;

namespace std
{
    template <class Iter>
    Iter begin(const std::pair<Iter, Iter>& range)
    {
        return range.first;
    }

    template <class Iter>
    Iter end(const std::pair<Iter, Iter>& range)
    {
        return range.second;
    }
}

namespace dci::module::ppn
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Node::Node()
        : idl::gen::ppn::Node<>::Opposite{idl::interface::Initializer{}}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Node::~Node()
    {
        dbgAssert(!_started);
        stop();
        _sol.flush();
        _tow.stop();
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::start(idl::gen::Config&& config)
    {
        config::ptree conf = config::cnvt(std::move(config));
        config::ptree nullConf{};

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        {
            _featureService.init();
            _featureService->started() += _sol * [this]
            {
                return cmt::readyFuture(_started);
            };

            _featureService->join() += _sol * [this](const api::link::Id& id, const transport::Address& a)
            {
                cmt::Promise<api::link::Remote<>> promise;
                cmt::Future<api::link::Remote<>> future = promise.future();
                _joinWaiters.emplace(a, std::move(promise));

                cmt::spawn() += _tow * [=, this]
                {
                    csessionWorker(id, a);
                };

                return future;
            };

            //Connectors
            _featureService->connect() += _sol * [this](const api::link::Id& id, const transport::Address& a)
            {
                cmt::spawn() += _tow * [=, this]
                {
                    csessionWorker(id, a);
                };
            };

            //RemoteAddressSpace
            _featureService->fireDiscovered() += _sol * [this](const api::link::Id& id, const transport::Address& a)
            {
                _featureService->discovered(id, a);
            };

            //LocalAddressSpace
            _featureService->getDeclared() += _sol * [this]()
            {
                return cmt::readyFuture(_declaredLocalAddresses);
            };

            _featureService->declare() += _sol * [this](const transport::Address& a){localAddressDeclare(a);};
            _featureService->undeclare() += _sol * [this](const transport::Address& a){localAddressUndeclare(a);};

            //AgentsRegistry
            _featureService->registerAgentProvider() += _sol * [this](idl::ILid ilid, api::feature::AgentProvider<>&& provider)
            {
                if(provider)
                {
                    provider.involvedChanged() += _sol * [this,ilid](bool v)
                    {
                        if(!v)
                        {
                            _agentRegistry.erase(ilid);
                        }
                    };
                    _agentRegistry.emplace(ilid, std::move(provider));
                }
                else
                {
                    _agentRegistry.erase(ilid);
                }
            };

            _featureService->getAgent() += _sol * [this](idl::ILid ilid)
            {
                auto iter = _agentRegistry.find(ilid);
                if(_agentRegistry.end() == iter)
                {
                    return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::Error>("no agent registred for requested ilid"));
                }

                return iter->second->getAgent(ilid);
            };
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //features
        List<api::link::Feature<>> linkFeatures;
        List<api::rdb::Feature<>> rdbFeatures;
        for(auto& p : conf.get_child("features", nullConf))
        {
            try
            {
                LOGI("setup feature: " << p.first);

                idl::Interface f = dciModuleEntry->manager()->createService(p.first).value();

                {
                    idl::gen::Configurable<> c = f;
                    if(c)
                    {
                        c->configure(config::cnvt(p.second)).value();
                    }
                }

                {
                    api::link::Feature<> linkf = f;
                    if(linkf)
                    {
                        linkFeatures.emplace_back(std::move(linkf));
                    }
                }

                {
                    api::rdb::Feature<> rdbf = f;
                    if(rdbf)
                    {
                        rdbFeatures.emplace_back(std::move(rdbf));
                    }
                }

                {
                    api::Feature<> nodef = f;
                    if(nodef)
                    {
                        nodef->setup(_featureService);
                    }
                }

                _features.emplace_back(std::move(f));
            }
            catch(...)
            {
                ExceptionPtr e = std::current_exception();
                std::rethrow_exception(exception::buildInstance<api::Error>(std::current_exception(), "unable to initialize feature '"+p.first+"'"));
            }
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //link
        {
            api::link::Key key = node::utils::parseKey(conf.get_child("key", nullConf));
            _link = dciModuleEntry->manager()->createService<api::link::Local<>>().value();
            _link->setKey(key);
            _link->setFeatures(std::move(linkFeatures));
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //rdb
        {
            api::rdb::Factory<> rdbFactory = dciModuleEntry->manager()->createService<api::rdb::Factory<>>().value();
            _rdbInstance = rdbFactory->build(std::move(rdbFeatures)).value();
            rdbFeatures.clear();
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //transport connctors
        {
            _connectors.loAdded() += _sol * [this](const transport::Address& a)
            {
                _featureService->connectorStarted(a);
            };
            _connectors.loDeleted() += _sol * [this](const transport::Address& a)
            {
                _featureService->connectorStopped(a);
            };

            _connectors.start(
                        dciModuleEntry->manager()->createService<transport::Connector<>>().value(),
                        [this](const transport::Address& a){return fixConnectorAddress(a);},
                        [this](const transport::Address& a){return makeConnector(a);},
                        conf.get_child("connect", nullConf),
                        [this]()->node::NetEnumerator&{return netEnumerator();});
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //transport acceptors
        {
            _acceptors.loAdded() += _sol * [](const transport::Address& a)
            {
                (void)a;
            };
            _acceptors.loDeleted() += _sol * [](const transport::Address& a)
            {
                (void)a;
            };

            _acceptors.start(
                        dciModuleEntry->manager()->createService<transport::Acceptor<>>().value(),
                        [this](const transport::Address& a){return fixAcceptorAddress(a);},
                        [this](const transport::Address& a){return makeAcceptor(a);},
                        conf.get_child("accept", nullConf),
                        [this]()->node::NetEnumerator&{return netEnumerator();});

            transport::Acceptor<> ah = _acceptors.hi();

            //out started(transport::Address);
            ah->started() += _sol * [=,this](const transport::Address& a1, const transport::Address& a2)
            {
                _featureService->acceptorStarted(a1, a2);
                localAddressDeclare(a2);

                auto nmIter = _nattMappings.find(a2);
                if(_nattMappings.end() == nmIter && _natt)
                {
                    _natt->mapping().then() += _sol * [=,this](cmt::Future<transport::natt::Mapping<>> in)
                    {
                        if(!_started) return;

                        if(in.resolvedValue())
                        {
                            _nattMappings.emplace(std::piecewise_construct_t{},
                                                  std::tie(a2),
                                                  std::forward_as_tuple(this, in.detachValue(), a2));
                        }
                        else if(in.resolvedException())
                        {
                            LOGW("mapping failed: "<<exception::toString(in.detachException()));
                        }
                        else //if(in.resolvedCancel())
                        {
                            LOGW("mapping canceled");
                        }
                    };
                }
            };

            //out stopped(transport::Address);
            ah->stopped() += _sol * [=,this](const transport::Address& a1, const transport::Address& a2)
            {
                _featureService->acceptorStopped(a1, a2);
                localAddressUndeclare(a2);

                _nattMappings.erase(a2);
            };

            //out failed(transport::Address, exception);
            ah->failed() += _sol * [this](const transport::Address& a1, const transport::Address& a2, const ExceptionPtr& e)
            {
                _featureService->acceptorFailed(a1, a2, e);
            };

            ah->accepted() += _sol * [this](transport::Channel<>&& ch)
            {
                cmt::spawn() += _tow * [ch=std::move(ch),this]() mutable
                {
                    asessionWorker(std::move(ch));
                };
            };
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //transport natt
        {
            _natt = dciModuleEntry->manager()->createService<transport::Natt<>>().value();
            _natt->configure(config::cnvt(conf.get_child("natt", nullConf)));
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //see net
        if(_netEnumerator)
        {
            _netEnumerator->start();
        }

        _started = true;

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //start features
        _featureService->start();

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        //listen
        _acceptors.hi()->start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::stop()
    {
        _started = false;
        _sol.flush();

        for(const auto&[i, m] : _nattMappings)
        {
            m._api->stop();
        }
        _nattMappings.clear();

        _connectionsInProgress.clear();
        _joinWaiters.clear();

        if(_featureService)
        {
            _featureService->stop();
        }

        _tow.flush();

        if(auto a = _acceptors.hi(); a)
        {
            a->stop();
        }
        _acceptors.stop();
        _connectors.stop();
        _features.clear();

        _tow.flush();

        _featureService.reset();
        _rdbInstance.reset();
        _link.reset();
        _natt.reset();
        _netEnumerator.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::localAddressDeclare(const transport::Address& a)
    {
        if(_declaredLocalAddresses.emplace(a).second)
        {
            if(_started) _featureService->declared(a);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::localAddressUndeclare(const transport::Address& a)
    {

        auto iter = _declaredLocalAddresses.find(a);
        if(_declaredLocalAddresses.end() != iter)
        {
            _declaredLocalAddresses.erase(iter);
            if(_started) _featureService->undeclared(a);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::emitFail(const std::string& comment)
    {
        auto e = exception::buildInstance<api::Error>(comment);
        if(_started) _featureService->failed(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::emitFail(ExceptionPtr e, const std::string& comment)
    {
        e = exception::buildInstance<api::Error>(std::move(e), comment);
        if(_started) _featureService->failed(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    node::NetEnumerator& Node::netEnumerator()
    {
        if(!_netEnumerator)
        {
            _netEnumerator.reset(new node::NetEnumerator());
            _netEnumerator->failed() += _sol * [this](ExceptionPtr&& e)
            {
                emitFail(std::move(e), "net enumerator failed");
            };
        }

        return *_netEnumerator;
    }

    namespace
    {
        std::string fixAuto(const std::string& src, const std::string& repl)
        {
            size_t pos = src.find("%auto%");
            if(src.npos == pos)
            {
                return src;
            }

            std::string res = src;
            res.replace(pos, 6, repl);

            return res;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    transport::Address Node::fixAcceptorAddress(const transport::Address& a)
    {
        dci::utils::URI<> uri;
        if(dci::utils::uri::parse(a.value, uri))
        {
            if(std::holds_alternative<dci::utils::uri::Inproc<>>(uri))
                return transport::Address{fixAuto(a.value, node::utils::mkRandomName(32))};

            if(std::holds_alternative<dci::utils::uri::Local<>>(uri))
                return transport::Address{fixAuto(a.value, "dci-ppn-node-"+node::utils::mkRandomName(32)+".sock")};
        }

        return a;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    transport::Address Node::fixConnectorAddress(const transport::Address& a)
    {
        dci::utils::URI<> uri;
        if(dci::utils::uri::parse(a.value, uri))
        {
            if(std::holds_alternative<dci::utils::uri::Inproc<>>(uri))
                return transport::Address{fixAuto(a.value, std::string{})};

            if(std::holds_alternative<dci::utils::uri::Local<>>(uri))
                return transport::Address{fixAuto(a.value, std::string{})};
        }

        return a;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    transport::acceptor::Downstream<> Node::makeAcceptor(const transport::Address& a)
    {
        dci::utils::URI<> uri;
        if(!dci::utils::uri::parse(a.value, uri))
        {
            emitFail("malformed address: "+a.value);
            dbgWarn("malformed address");
            return transport::acceptor::Downstream<>();
        }

        return std::visit([&]<class Alt>(const Alt& /*alt*/)
                          {
                              if constexpr(std::is_same_v<dci::utils::uri::TCP<>, Alt> || std::is_base_of_v<dci::utils::uri::TCP<>, Alt>)
                              {
                                  transport::net::Acceptor<> res = dciModuleEntry->manager()->createService<transport::net::Acceptor<>>().value();
                                  res->bind(a).value();
                                  return transport::acceptor::Downstream<>(res);
                              }

                              if constexpr(std::is_same_v<dci::utils::uri::Local<>, Alt>)
                              {
                                  transport::net::Acceptor<> res = dciModuleEntry->manager()->createService<transport::net::Acceptor<>>().value();
                                  res->bind(a).value();
                                  return transport::acceptor::Downstream<>(res);
                              }

                              if constexpr(std::is_same_v<dci::utils::uri::Inproc<>, Alt>)
                              {
                                  transport::inproc::Acceptor<> res = dciModuleEntry->manager()->createService<transport::inproc::Acceptor<>>().value();
                                  res->bind(a).value();
                                  return transport::acceptor::Downstream<>(res);
                              }

                              emitFail("malformed address: "+a.value);
                              dbgWarn("malformed address");
                              return transport::acceptor::Downstream<>();

                          }, uri);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    transport::connector::Downstream<> Node::makeConnector(const transport::Address& a)
    {
        dci::utils::URI<> uri;
        if(!dci::utils::uri::parse(a.value, uri))
        {
            emitFail("malformed address: "+a.value);
            dbgWarn("malformed address");
            return transport::acceptor::Downstream<>();
        }

        return std::visit([&]<class Alt>(const Alt& /*alt*/)
                          {
                              if constexpr(std::is_same_v<dci::utils::uri::TCP<>, Alt> || std::is_base_of_v<dci::utils::uri::TCP<>, Alt>)
                              {
                                  transport::net::Connector<> res = dciModuleEntry->manager()->createService<transport::net::Connector<>>().value();
                                  res->bind(a).value();
                                  return transport::connector::Downstream<>(res);
                              }

                              if constexpr(std::is_same_v<dci::utils::uri::Local<>, Alt>)
                              {
                                  transport::net::Connector<> res = dciModuleEntry->manager()->createService<transport::net::Connector<>>().value();
                                  res->bind(a).value();
                                  return transport::connector::Downstream<>(res);
                              }

                              if constexpr(std::is_same_v<dci::utils::uri::Inproc<>, Alt>)
                              {
                                  transport::inproc::Connector<> res = dciModuleEntry->manager()->createService<transport::inproc::Connector<>>().value();
                                  return transport::connector::Downstream<>(res);
                              }

                              emitFail("malformed address: "+a.value);
                              dbgWarn("malformed address");
                              return transport::connector::Downstream<>();

                          }, uri);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::csessionWorker(api::link::Id id, const transport::Address& a)
    {
        if(!_connectionsInProgress.insert(a).second)
        {
            //connection already in progress
            return;
        }

        api::feature::CSession<>::Opposite s{idl::interface::Initializer{}};

        s->address() += [a]
        {
            return cmt::readyFuture(a);
        };

        sbs::Owner sbsOwner4Id;
        s->id() += sbsOwner4Id * [id]
        {
            return cmt::readyFuture(id);
        };

        utils::AtScopeExit sg{[&,this]
        {
            if(s)
            {
                s->closed();
            }
            _connectionsInProgress.erase(a);
        }};

        _featureService->newSession(id, a, s.opposite());

        transport::Channel<> ch;
        try
        {
            ch = _connectors.hi()->connect(a).value();
            _connectionsInProgress.erase(a);
            s->connected();
        }
        catch(const cmt::task::Stop&)
        {
            auto e = exception::buildInstance<api::Error>("node stopped");
            flushJoinWaiters(a, e);
            s->failed(e);
            return;
        }
        catch(...)
        {
            auto e = exception::buildInstance<api::Error>(std::current_exception());
            flushJoinWaiters(a, e);
            s->failed(e);
            return;
        }

        try
        {
            api::link::Remote<> r = _link->joinByConnect(std::move(ch)).value();
            api::link::Id id2 = r->id().value();

            if(id != id2)
            {
                sbsOwner4Id.flush();
                s->id() += [id2]
                {
                    return cmt::readyFuture(id2);
                };

                id = id2;
                s->idSpecified(id);
            }

            s->joined(r);
            flushJoinWaiters(a, r);
            _rdbInstance->addRemote(id2, r);

            r->closed() += _sol * [s]() mutable
            {
                s->closed();
            };

            s.reset();
        }
        catch(const cmt::task::Stop&)
        {
            auto e = exception::buildInstance<api::Error>("node stopped");
            flushJoinWaiters(a, e);
            s->failed(e);
            return;
        }
        catch(...)
        {
            auto e = exception::buildInstance<api::Error>(std::current_exception());
            flushJoinWaiters(a, e);
            s->failed(e);
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::asessionWorker(transport::Channel<>&& ch)
    {
        api::feature::ASession<>::Opposite s{idl::interface::Initializer{}};

        s->address() += [removeAddress=ch->remoteAddress()] () mutable
        {
            return removeAddress;
        };

        sbs::Owner sbsOwner4Id;
        s->id() += sbsOwner4Id * []
        {
            return cmt::readyFuture(api::link::Id{});
        };

        utils::AtScopeExit sg{[&]
        {
            if(s)
            {
                s->closed();
            }
        }};

        api::feature::Acceptors<>::Opposite{_featureService}->newSession(s.opposite());

        try
        {
            api::link::Remote<> r = _link->joinByAccept(std::move(ch)).value();
            api::link::Id id = r->id().value();

            sbsOwner4Id.flush();
            s->id() += [id]
            {
                return cmt::readyFuture(id);
            };

            s->idSpecified(id);
            s->joined(r);
            _rdbInstance->addRemote(id, r);

            r->closed() += _sol * [s]() mutable
            {
                s->closed();
            };

            s.reset();
        }
        catch(const cmt::task::Stop&)
        {
            s->failed(exception::buildInstance<api::Error>("node stopped"));
        }
        catch(...)
        {
            s->failed(exception::buildInstance<api::Error>(std::current_exception()));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::flushJoinWaiters(const transport::Address& a, ExceptionPtr e)
    {
        for(auto iter{_joinWaiters.lower_bound(a)}; iter != _joinWaiters.end() && iter->first == a; )
        {
            iter->second.resolveException(e);
            iter = _joinWaiters.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Node::flushJoinWaiters(const transport::Address& a, api::link::Remote<> r)
    {
        for(auto iter{_joinWaiters.lower_bound(a)}; iter != _joinWaiters.end() && iter->first == a; )
        {
            iter->second.resolveValue(r);
            iter = _joinWaiters.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Node::Mapping::Mapping(Node* node, transport::natt::Mapping<>&& api, const transport::Address& internal)
        : _node{node}
        , _api{std::move(api)}
    {
        _api->established() += _sbsOwner * [internal,this](const transport::Address& external)
        {
            if(!_external.value.empty())
            {
                LOGI("natt unmapped "<<internal.value<<" <- "<<_external.value);
                _node->localAddressUndeclare(_external);
            }
            _external = external;
            if(!_external.value.empty())
            {
                LOGI("natt mapped "<<internal.value<<" <- "<<_external.value);
                _node->localAddressDeclare(external);
            }
        };

        _api->unestablished() += _sbsOwner * [internal,this]()
        {
            if(!_external.value.empty())
            {
                LOGI("natt unmapped "<<internal.value<<" <- "<<_external.value);
                _node->localAddressUndeclare(std::exchange(_external, {}));
            }
        };

        _api.involvedChanged() += _sbsOwner * [internal,this](bool v)
        {
            if(!v)
            {
                _sbsOwner.flush();
                _api.reset();
                if(!_external.value.empty())
                {
                    LOGI("natt unmapped "<<internal.value<<" <- "<<_external.value);
                    _node->localAddressUndeclare(std::exchange(_external, {}));
                }
            }
        };

        _api->start(internal, transport::natt::Protocol::tcp);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Node::Mapping::~Mapping()
    {
        _sbsOwner.flush();

        if(_api)
        {
            _api->stop();
            _api.reset();
        }
        if(!_external.value.empty())
        {
            _node->localAddressUndeclare(std::exchange(_external, {}));
        }
    }
}
