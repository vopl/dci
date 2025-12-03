// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "joining.hpp"

namespace dci::module::ppn::connectivity
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Joining::Joining()
        : api::Joining<>::Opposite{idl::interface::Initializer{}}
    {
        {
            node::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](node::feature::Service<> srv)
            {
                _nodeSrv = srv;

                srv->start() += serviceSol() * [this, srv]() mutable
                {
                    _started = true;
                };

                srv->stop() += serviceSol() * [this]
                {
                    _started = false;
                    _instances.clear();
                };

                srv->getAgent(api::demand::Registry<>::lid()).then() += serviceSol() * [this](cmt::Future<idl::Interface> in)
                {
                    if(in.resolvedValue())
                    {
                        _demandRegistry = in.detachValue();

                        if(!_demandRegistry)
                        {
                            LOGW("null demand registry provided");
                            return;
                        }

                        _demandRegistry->satisfy() += serviceSol() * [this](const node::link::Id& id, api::demand::SatisfactionHolder<>&& sh)
                        {
                            if(_started)
                            {
                                satisfyDemand(id, std::move(sh));
                            }
                        };
                    }

                    if(in.resolvedException())
                    {
                        LOGW("unable to obtain demand registry: "<<exception::toString(in.detachException()));
                    }
                };

                srv->discovered() += serviceSol() * [this](const node::link::Id& id, const auto&)
                {
                    if(!_started) return;

                    auto iter = _instances.find(id);
                    if(_instances.end() != iter)
                    {
                        iter->discovered();
                    }
                };
            };
        }

        {
            node::link::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](node::link::feature::Service<> srv)
            {
                auto onJoined = [this](const node::link::Id& id, node::link::Remote<> r)
                {
                    if(!_started) return;

                    auto iter = _instances.find(id);
                    if(_instances.end() != iter)
                    {
                        iter->joined(r);
                    }
                };

                srv->joinedByConnect() += serviceSol() * onJoined;
                srv->joinedByAccept() += serviceSol() * onJoined;
            };
        }

        {
            node::rdb::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](node::rdb::feature::Service<> srv)
            {
                _rdbQuery = srv;
                return cmt::readyFuture(List<rdb::pql::Column>{});
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Joining::~Joining()
    {
        serviceSol().flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Joining::satisfyDemand(const node::link::Id& id, api::demand::SatisfactionHolder<>&& sh)
    {
        if(!_started) return;

        auto iter = _instances.lower_bound(id);

        if(_instances.end() == iter || id != iter->id())
        {
            iter = _instances.emplace_hint(iter, this, id);
        }

        dbgAssert(id == iter->id());
        iter->demand(std::move(sh));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Joining::instanceEmpty(const node::link::Id& id)
    {
        if(!_started) return;

        auto iter = _instances.find(id);
        if(_instances.end() != iter)
        {
            _instances.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<Tuple<node::link::Remote<>, transport::Address, real64>> Joining::fetchRdbRecord(const node::link::Id& id)
    {
        if(!_started) return cmt::cancelledFuture<Tuple<node::link::Remote<>, transport::Address, real64>>();

        static const List<rdb::pql::Column> columns{{api::Reest<>::lid(), "address"}, rdb::pql::Column{api::Reest<>::lid(), "rating"}};

        return _rdbQuery->record(id, columns).chain() += serviceSol() * [](
                                                   cmt::Future<Tuple<node::link::Remote<>, List<rdb::pql::Value>>> in,
                                                   cmt::Promise<Tuple<node::link::Remote<>, transport::Address, real64>>& out)
        {
            auto [remote, values] = in.detachValue();

            out.resolveValue(std::forward_as_tuple(
                    std::move(remote),
                    transport::Address{values[0].data.getOr(String{})},
                    values[1].data.getOr(real64{})));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<node::link::Remote<>> Joining::join(const node::link::Id& id, transport::Address&& a)
    {
        if(!_started) return cmt::cancelledFuture<node::link::Remote<>>();

        return _nodeSrv->join(id, a);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Joining::CmpById::operator()(const joining::Instance& a, const joining::Instance& b) const
    {
        return a.id() < b.id();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Joining::CmpById::operator()(const joining::Instance& a, const node::link::Id& b) const
    {
        return a.id() < b;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Joining::CmpById::operator()(const node::link::Id& a, const joining::Instance& b) const
    {
        return a < b.id();
    }
}
