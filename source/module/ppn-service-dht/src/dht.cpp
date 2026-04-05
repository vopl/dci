/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "dht.hpp"

namespace dci::module::ppn::service
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    auto Dht::awaitReady(auto&& f)
    {
        if(_ready.isRaised())
        {
            return f();
        }

        struct WaitableTimer : poll::WaitableTimer<>, mm::heap::Allocable<WaitableTimer>
        {
            using poll::WaitableTimer<>::WaitableTimer;
        };
        std::unique_ptr<WaitableTimer> timeout = std::make_unique<WaitableTimer>(std::chrono::seconds{5});
        timeout->start();

        typename decltype(f())::Promise promise;
        decltype(f()) future = promise.future();

        cmt::whenAny(_ready, *timeout, future.waitable()).then() += [this, f=std::forward<decltype(f)>(f), promise=std::move(promise), timeout=std::move(timeout)]() mutable
        {
            if(promise.resolved())
            {
                return;
            }

            if(_ready.isRaised())
            {
                promise.continueAs(f());
                return;
            }

            promise.resolveException(exception::buildInstance<api::NotReady>());
        };

        return future;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Dht::Dht()
        : idl::gen::ppn::service::Dht<>::Opposite{idl::interface::Initializer{}}
    {
//        {
//            static std::vector<Dht*> dhts;

//            dhts.push_back(this);

//            if(1 == dhts.size())
//            {
//                cmt::spawn() += []
//                {
//                    poll::Timer ticker{std::chrono::milliseconds{5}, true};
//                    ticker.start();
//                    for(;;)
//                    {
//                        ticker.wait();

//                        Dht* initiator = dhts[rand() % dhts.size()];

//                        std::vector<uint64> dists;

//                        static int advances = 0;
//                        static int count = 0;
//                        api::ProgressListner<>::Opposite pl{idl::interface::Initializer{}};
//                        pl.advance() += [&](uint64 dist)
//                        {
//                            //std::cout << "------ progress: " << dist <<std::endl;
//                            dists.push_back(dist);
//                            advances++;
//                        });

//                        api::Key key{};
//                        for(auto& v : key.value)
//                        {
//                            v = rand();
//                        }

//                        initiator->onLocalPut(key, api::Value{}, pl.opposite()).wait();

//                        count++;
//                        //std::cout << "------ done, " << real64(advances-count)/real64(count) <<std::endl;

//                        if(dists.size() >= 5)
//                        {
//                            std::size_t cnt = 0;
//                            std::cout << "dists: ";
//                            for(auto iter = dists.rbegin(); cnt < 5; ++iter, ++cnt)
//                            {
//                                std::cout << *iter << " ";
//                            }
//                            std::cout << std::endl;
//                        }
//                        dists.clear();

//                    }
//                };
//            }
//        }

        {
            //in put(Key, Value, ProgressListner) -> void;
            _local->put() += serviceSol() * [this](const api::Key& key, api::Value&& value, api::ProgressListner<>&& pl)
            {
                return onLocalPut(key, std::move(value), std::move(pl));
            };

            //in get(Key, ProgressListner) -> Value;
            _local->get() += serviceSol() * [this](const api::Key& key, uint32 limit, api::ProgressListner<>&& pl)
            {
                return onLocalGet(key, limit, std::move(pl));
            };

            _local.involvedChanged() += serviceSol() * [this](bool)
            {
                updateReadyStatus();
            };
        }

        //node::feature::AgentProvider
        //in getAgent(ilid) -> interface;
        methods()->getAgent() += serviceSol() * [this](idl::ILid ilid)
        {
            if(api::Processor<>::lid() == ilid)
            {
                return cmt::readyFuture(idl::Interface{_local.opposite()});
            }

            dbgWarn("crazy node?");
            return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::Error>("bad agent ilid requested"));
        };

        {
            node::Feature<>::Opposite op = *this;
            op->setup() += serviceSol() * [this](const node::feature::Service<>& srv)
            {
                srv->start() += serviceSol() * [this]
                {
                    _nodeStarted  = true;
                    updateReadyStatus();
                };

                srv->stop() += serviceSol() * [this]
                {
                    _nodeStarted  = false;
                    updateReadyStatus();
                };

                srv->registerAgentProvider(api::Processor<>::lid(), node::feature::AgentProvider<>{*this});
            };
        }

        {
            link::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](link::feature::Service<> srv)
            {
                srv->id().then() += serviceSol() * [this](cmt::Future<link::Id> in)
                {
                    if(in.resolvedValue())
                    {
                        _localId = in.value();
                        _localIdSetted = true;
                    }
                    else if(in.resolvedException())
                    {
                        LOGE("link local.id failed: "<<exception::toString(in.detachException()));
                        _localIdSetted = false;
                    }
                    else
                    {
                        LOGE("link local.id canceled");
                        _localIdSetted = false;
                    }

                    updateReadyStatus();
                };

                srv->addPayload(*this);

                auto onJoin = [this](const link::Id& id, link::Remote<> r)
                {
                    r->getInstance(api::internal::Peer<>::lid()).then() += serviceSol() * [id, this](cmt::Future<idl::Interface> in)
                    {
                        if(in.resolvedValue())
                        {
                            auto res = _remotes.emplace(id, in.detachValue());
                            dbgAssert(res.second);
                            if(res.second)
                            {
                                const Remote& remote = *res.first;
                                remote._api.involvedChanged() += serviceSol() * [this, api=remote._api.weak()](bool v)
                                {
                                    if(!v)
                                    {
                                        _remotes.get<RemoteByApi>().erase(api);
                                        updateReadyStatus();
                                    }
                                };
                                updateReadyStatus();
                            }
                        }
                    };
                };

                srv->joinedByConnect() += serviceSol() * onJoin;
                srv->joinedByAccept() += serviceSol() * onJoin;
            };
        }

        {
            link::feature::Payload<>::Opposite op = *this;

            //in ids() -> set<ilid>;
            op->ids() += serviceSol() * []()
            {
                return cmt::readyFuture(Set<idl::interface::Lid>{
                                            api::internal::Peer<>::lid()});
            };

            //in getInstance(Id requestorId, Remote requestor, ilid) -> interface;
            op->getInstance() += serviceSol() * [this](const link::Id&, const link::Remote<>&, idl::interface::Lid ilid)
            {
                if(api::internal::Peer<>::lid() == ilid)
                {
                    return cmt::readyFuture(idl::Interface{_local.opposite()});
                }

                dbgWarn("crazy link?");
                return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::Error>("bad instance ilid requested"));
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Dht::~Dht()
    {
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Dht::updateReadyStatus()
    {
        bool ready = _nodeStarted  && _localIdSetted && _local.involved() && _remotes.size() >= 2;

        if(ready)
        {
            _ready.raise();
        }
        else
        {
            _ready.reset();
        }
    }

    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        inline uint64 distance(const Array<uint8, 32>& key1, const Array<uint8, 32>& key2)
        {
            uint64 pos1 = *static_cast<const uint64*>(static_cast<const void *>(key1.data() + key1.size() - sizeof(uint64)));
            uint64 pos2 = *static_cast<const uint64*>(static_cast<const void *>(key2.data() + key2.size() - sizeof(uint64)));
            return pos1 > pos2 ?
                        pos1 - pos2 :
                        pos2 - pos1;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<None> Dht::onLocalPut(const api::Key& key, api::Value&& value, api::ProgressListner<>&& pl)
    {
        return awaitReady([this, key=key, value=std::move(value), pl=std::move(pl)]() mutable
        {
            if(pl)
            {
                dbgAssert(_localIdSetted);
                pl->advance(distance(_localId, key.value));
            }

            const Remote* next = moreClosest(key);
            if(next)
            {
                return next->_api->put(key, std::move(value), std::move(pl));
            }
            else
            {
                _storage.put(key, value);
            }

            pl.reset();
            return cmt::readyFuture(None{});
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<Set<idl::gen::ppn::service::dht::Value> > Dht::onLocalGet(const api::Key& key, uint32 limit, api::ProgressListner<>&& pl)
    {
        return awaitReady([this, key=key, limit, pl=std::move(pl)]() mutable
        {
            if(pl)
            {
                dbgAssert(_localIdSetted);
                pl->advance(distance(_localId, key.value));
            }

            Set<api::Value> res;
            _storage.get(key, limit, res);

            if(res.size() >= limit)
            {
                pl.reset();
                return cmt::readyFuture(std::move(res));
            }

            const Remote* next = moreClosest(key);
            if(next)
            {
                return next->_api->get(key, limit, std::move(pl)).chain() += serviceSol() * [res=std::move(res)](cmt::Future<Set<api::Value>>&& in, cmt::Promise<Set<api::Value>>& out) mutable
                {
                    if(in.resolvedValue())
                    {
                        res.merge(in.detachValue());
                        out.resolveValue(std::move(res));
                    }
                    else if(!res.empty())
                    {
                        out.resolveValue(std::move(res));
                    }
                    else
                    {
                        out.resolveAs(std::move(in));
                    }
                };
            }

            pl.reset();
            return cmt::readyFuture(std::move(res));
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Dht::Remote* Dht::moreClosest(const api::Key& key)
    {
        dbgAssert(_localIdSetted);

        if(_remotes.empty())
        {
            return {};
        }

        auto& idx = _remotes.get<RemoteById>();

        auto iter1 = idx.lower_bound(key.value);
        if(idx.end() == iter1)
        {
            --iter1;
        }
        auto iter2 = iter1;
        if(idx.begin() != iter2)
        {
            --iter2;
        }

        uint64 dist1 = distance(key.value, iter1->_id);
        uint64 dist2 = distance(key.value, iter2->_id);
        uint64 distMy = distance(key.value, _localId);

        if(distMy < dist1 && distMy < dist2)
        {
            return {};
        }

        return (dist1 < dist2) ?
                    &*iter1 :
                    &*iter2;
    }
}
