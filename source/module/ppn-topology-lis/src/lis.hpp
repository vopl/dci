// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "lis/grid.hpp"
#include "lis/io.hpp"
#include "lis/space.hpp"

namespace dci::module::ppn::topology
{
    class Lis
        : public idl::gen::ppn::topology::Lis<>::Opposite
        , public host::module::ServiceBase<Lis>
    {
    public:
        Lis();
        ~Lis();

    private:
        void initRdb(rdb::Query<>&& rdbQuery);

    private:
        void changed(const lis::space::Id& id);
        void state(const lis::space::Id& id, real64 rating, List<transport::Address>&& addresses);
        void joined(const lis::space::Id& id, const node::link::Remote<>& r);
        void disjoined(const lis::space::Id& id, const node::link::Remote<>& r);

    public:
        bool started() const;
        const lis::space::Id& localId() const;

        lis::space::Csid csid(const lis::space::Id& id);

        void requestState(const lis::space::Id& id);
        void requestDemand(const lis::space::Id& id, real64 weight);

        void supplied(const lis::space::Id& id, transport::Address&& addr);

    public:
        void requestState(lis::space::Csid csidStart, lis::space::Csid csidStop);

    private:
        void gridRequestTick();

        void setIntensity(double v);

    private:
        node::feature::RemoteAddressSpace<> _ras;
        bool                                _started {};
        lis::space::Id                      _localId {};
        bool                                _localIdSetted {};

        rdb::Query<>                        _rdbQuery;
        rdb::query::Result<>                _rdbQueryResult4All;

        demand::Registry<>                  _demandRegistry;

    private:
        lis::Grid       _grid;
        poll::Timer     _gridRequestTicker {std::chrono::seconds{10}, true, [this]{gridRequestTick();}};
        lis::Io         _io;
    };
}
