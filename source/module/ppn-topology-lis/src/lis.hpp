/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
