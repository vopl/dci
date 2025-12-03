// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "lis.hpp"

namespace dci::module::ppn::topology
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lis::Lis()
        : idl::gen::ppn::topology::Lis<>::Opposite{idl::interface::Initializer()}
        , _grid{this}
        , _io{this}
    {
        {
            node::link::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](const node::link::feature::Service<>& srv)
            {
                srv->addPayload(*this);

                srv->joinedByConnect() += serviceSol() * [this](const lis::space::Id& id, const node::link::Remote<>& r)
                {
                    if(!_started || !_localIdSetted) return;

                    joined(id, r);
                };

                srv->joinedByAccept() += serviceSol() * [this](const lis::space::Id& id, const node::link::Remote<>& r)
                {
                    if(!_started || !_localIdSetted) return;

                    joined(id, r);
                };

                srv->id().then() += serviceSol() * [this](auto in)
                {
                    if(in.resolvedValue())
                    {
                        _localId = in.value();
                        _localIdSetted = true;

                        if(_started)
                        {
                            _grid.start();
                            _io.start();
                            _gridRequestTicker.start();
                        }
                    }
                    else if(in.resolvedException())
                    {
                        LOGE("link local.id failed: "<<exception::toString(in.detachException()));
                    }
                    else
                    {
                        LOGE("link local.id canceled");
                    }
                };
            };
        }

        {
            node::link::feature::Payload<>::Opposite op = *this;

            //in ids() -> set<ilid>;
            op->ids() += serviceSol() * []()
            {
                return cmt::readyFuture(Set<idl::interface::Lid>{api::Supplier<>::lid()});
            };

            //in getInstance(Id requestorId, Remote requestor, ilid) -> interface;
            op->getInstance() += serviceSol() * [this](const lis::space::Id& id, const node::link::Remote<>& r, idl::interface::Lid ilid)
            {
                if(api::Supplier<>::lid() == ilid)
                {
                    return cmt::readyFuture(idl::Interface{_io.allocConsumer(id, r).opposite()});
                }

                dbgWarn("crazy link?");

                return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::Error>("bad instance ilid requested"));
            };
        }

        {
            rdb::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](const rdb::feature::Service<>& srv)
            {
                initRdb(rdb::Query<>{srv});
                return cmt::readyFuture(List<pql::Column>{});
            };
        }

        {
            node::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](const node::feature::Service<>& srv)
            {
                srv->start() += serviceSol() * [this]() mutable
                {
                    _started = true;

                    if(_rdbQueryResult4All)
                    {
                        _rdbQueryResult4All->start();
                    }

                    if(_localIdSetted)
                    {
                        _grid.start();
                        _io.start();
                        _gridRequestTicker.start();
                    }

                };

                srv->stop() += serviceSol() * [this]
                {
                    _started = false;

                    _gridRequestTicker.stop();
                    _grid.stop();
                    _io.stop();

                    if(_rdbQueryResult4All)
                    {
                        _rdbQueryResult4All->stop();
                    }
                };

                srv->getAgent(demand::Registry<>::lid()).then() += serviceSol() * [this](auto in)
                {
                    if(in.resolvedValue())
                    {
                        _demandRegistry = in.detachValue();
                        return;
                    }

                    LOGW(exception::toString(in.detachException()));
                };

                _ras = srv;

                _ras->discovered() += serviceSol() * [this](const lis::space::Id& id, transport::Address&& addr)
                {
                    if(!_started) return;

                    _io.discovered(id, std::move(addr));
                };
            };
        }

        {
            idl::gen::Configurable<>::Opposite op = *this;

            op->configure() += serviceSol() * [this](dci::idl::gen::Config&& config)
            {
                auto c = config::cnvt(std::move(config));

                _grid.setup(
                            c.get<uint8>("gridBits", lis::grid::Kernel::_bitsDefault),
                            c.get<uint16>("gridSize", lis::grid::Kernel::_sizeDefault));

                setIntensity(std::atof(c.get("intensity", "0.1").data()));

                return cmt::readyFuture(None{});
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::initRdb(rdb::Query<>&& rdbQuery)
    {
        _rdbQuery = std::move(rdbQuery);

        {
            _rdbQuery->continuous(
                        rdb::query::Scope::candidates | rdb::query::Scope::online,
                        pql::Expression{pql::Value{true}}).then() += serviceSol() * [this](cmt::Future<rdb::query::Result<>> in)
            {
                if(in.resolvedValue())
                {
                    _rdbQueryResult4All = in.detachValue();
                    _rdbQueryResult4All->next() += serviceSol() * [this](const lis::space::Id& id, const node::link::Remote<>&)
                    {
                        if(!_started) return;

                        changed(id);
                    };

                    if(_started)
                    {
                        _rdbQueryResult4All->start();
                    }
                }
                else if(in.resolvedException())
                {
                    LOGE("rdb query failed: "<<exception::toString(in.detachException()));
                }
                else
                {
                    LOGE("rdb query canceled");
                }
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Lis::~Lis()
    {
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::changed(const lis::space::Id& id)
    {
        if(!_started) return;

        _rdbQuery->record(
                    id,
                    List<pql::Column>{
                        pql::Column{connectivity::Reest<>::lid(), "rating"},
                        pql::Column{connectivity::Reest<>::lid(), "topAddresses"}
                    }).then() +=  serviceSol() * [id,this](cmt::Future<Tuple<node::link::Remote<>, List<pql::Value>>> in)
        {
            if(!_started) return;

            if(in.resolvedValue())
            {
                auto&&[r, vals] = in.detachValue();

                real64 rating = vals[0].data.getOr(real64{});

                List<pql::Value> avals = vals[1].data.getOr(List<pql::Value>{});
                List<transport::Address> addresses;
                for(pql::Value& v : avals)
                {
                    addresses.push_back(transport::Address{v.data.getOr(String{})});
                }

                state(id,
                      rating,
                      std::move(addresses));
            }
            else if(in.resolvedException())
            {
                LOGE("rdb query failed: "<<exception::toString(in.detachException()));
            }
            else
            {
                LOGE("rdb query canceled");
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::state(const lis::space::Id& id, real64 rating, List<transport::Address>&& addresses)
    {
        if(!_started) return;

        _grid.state(id, rating);
        _io.state(id, rating, std::move(addresses));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::joined(const lis::space::Id& id, const node::link::Remote<>& r)
    {
        if(!_started) return;

        r.involvedChanged() += serviceSol() * [id,r=r.weak(),this](bool v)
        {
            if(!v)
            {
                disjoined(id, r);
            }
        };

        r->getInstance(api::Supplier<>::lid()).then() += serviceSol() * [id,r=r.weak(),this](cmt::Future<idl::Interface> in)
        {
            if(!_started) return;

            if(!in.resolvedValue())
            {
                return;
            }

            api::Supplier<> s {in.detachValue()};
            if(!s)
            {
                return;
            }

            _io.allocSupplier(id, r, s);
        };

        _grid.joined(id, r);
        _io.joined(id, r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::disjoined(const lis::space::Id& id, const node::link::Remote<>& r)
    {
        if(!_started) return;
        _grid.disjoined(id, r);
        _io.disjoined(id, r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Lis::started() const
    {
        return _started;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const lis::space::Id& Lis::localId() const
    {
        return _localId;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    lis::space::Csid Lis::csid(const lis::space::Id& id)
    {
        return lis::space::csid(_localId, id);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::requestState(const lis::space::Id& id)
    {
        lis::space::Csid csid = this->csid(id);
        return requestState(csid, csid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::requestDemand(const lis::space::Id& id, real64 weight)
    {
        dbgAssert(weight >= 0);

        if(!_demandRegistry)
        {
            _grid.demand(id, weight, demand::NeedHolder<>{});
            return;
        }

        _demandRegistry->need(id, weight).then() += serviceSol() * [id,weight,this](cmt::Future<demand::NeedHolder<>> in)
        {
            if(in.resolvedValue())
            {
                _grid.demand(id, weight, in.detachValue());
            }
            else if(in.resolvedException())
            {
                LOGE("demand failed: "<<exception::toString(in.detachException()));
            }
            else
            {
                LOGE("demand canceled");
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::supplied(const lis::space::Id& id, transport::Address&& addr)
    {
        if(!_started) return;
        if(!_ras) return;

        _ras->fireDiscovered(id, std::move(addr));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::requestState(lis::space::Csid csidStart, lis::space::Csid csidStop)
    {
        pql::Node constraints =
        {
            pql::fun::bool_or,
            {
            }
        };

        for(const lis::space::Range& r : lis::space::ranges(_localId, csidStart, csidStop))
        {
            constraints.args.push_back(pql::Node
            {
                pql::fun::num_between,
                {
                    pql::Node
                    {
                        pql::fun::id_tip64,
                        {
                            pql::Column{idl::ILid{}, "id"},
                        }
                    },
                    pql::Value{r._min},
                    pql::Value{r._max},
                }
            });
        }

        _rdbQuery->select(
                    rdb::query::Scope::candidates | rdb::query::Scope::online,
                    constraints,
                    List<pql::Column>{
                        pql::Column{connectivity::Reest<>::lid(), "rating"},
                        pql::Column{connectivity::Reest<>::lid(), "topAddresses"}},
                    1024
                    ).then() += serviceSol() * [this](cmt::Future<List<Tuple<lis::space::Id, node::link::Remote<>, List<pql::Value>>>> in)
        {
            if(!_started) return;

            if(in.resolvedValue())
            {
                for(const auto&[id, r, vals] : in.value())
                {
                    (void)r;

                    real64 rating = vals[0].data.getOr(real64{});

                    List<pql::Value> avals = vals[1].data.getOr(List<pql::Value>{});
                    List<transport::Address> addresses;
                    for(pql::Value& v : avals)
                    {
                        addresses.push_back(transport::Address{v.data.getOr(String{})});
                    }

                    state(id,
                          rating,
                          std::move(addresses));
                }
            }
            else if(in.resolvedException())
            {
                LOGE("rdb query failed: "<<exception::toString(in.detachException()));
            }
            else
            {
                LOGE("rdb query canceled");
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::gridRequestTick()
    {
        List<api::GridFillingStep> filling;
        if(_grid.fillRequest(filling))
        {
            _io.ownGridRequest(_grid.kernel(), filling);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Lis::setIntensity(double v)
    {
        static constexpr int64 min = 50;
        static constexpr int64 max = int64{1000}*60*60*24;

        int64 ms;
        if(v <= 0)  ms = max;
        else        ms = std::clamp(static_cast<int64>(1000.0/v), min, max);

        _gridRequestTicker.interval(std::chrono::milliseconds{ms});
    }
}
