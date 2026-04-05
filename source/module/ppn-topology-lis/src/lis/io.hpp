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
#include "io/online.hpp"
#include "io/consumer.hpp"
#include "io/supplier.hpp"
#include "grid/kernel.hpp"
#include "space.hpp"

namespace dci::module::ppn::topology
{
    class Lis;
}

namespace dci::module::ppn::topology::lis
{
    class Io
    {
    public:
        Io(Lis* lis);
        ~Io();

        void start();
        void stop();

        uint16 amount4NearsSubscription() const;

        void state(const space::Id& id, real64 rating, List<transport::Address>&& addresses);

        void discovered(const space::Id& id, transport::Address&& address);

        void joined(const space::Id& id, const node::link::Remote<>& r);
        void disjoined(const space::Id& id, const node::link::Remote<>& r);

        api::Supplier<>::Opposite allocConsumer(const space::Id& id, const node::link::Remote<>& r);
        void freeConsumer(const space::Id& id, const api::Supplier<>::Opposite& s);

        void allocSupplier(const space::Id& id, const node::link::Remote<>& r, const api::Supplier<>& s);
        void freeSupplier(const space::Id& id, const api::Supplier<>& s);

    public:
        void supplied(const transport::Address& remoteAddr, const space::Id& remoteId, List<transport::Address>&& as);
        void started(io::Consumer* consumer);
        void charged(io::Online* online);
        void changed(io::Online* online);

        void remoteGridRequest(io::Consumer* consumer, const io::Consumer::GridRequest& request);
        std::size_t remoteGridRequest(io::Consumer* consumer, std::size_t minGridIdx, std::size_t maxGridIdx, std::size_t amount);
        void ownGridRequest(const grid::Kernel& gridKernel, const List<api::GridFillingStep>& filling);

    private:
        template <class Ring, class F>
        std::size_t enumerateNears(Ring& ring, space::Id center, space::SmallNum radius, std::size_t amount, F&& f);

    private:
        Lis* _lis;

        struct IdCmp
        {
            bool operator()(const space::Id& a, const space::Id& b) const
            {
                constexpr uint32 size = space::Id{}.size();
                for(uint32 idx{size-1}; idx < size; --idx)
                {
                    if(a[idx] == b[idx])
                    {
                        continue;
                    }
                    return a[idx] < b[idx];
                }

                return false;
            }
        };

        using Onlines   = std::map<space::Id, io::Online,   IdCmp>;
        using Consumers = std::map<space::Id, io::Consumer, IdCmp>;
        using Suppliers = std::map<space::Id, io::Supplier, IdCmp>;

        Onlines     _onlines;
        Consumers   _consumers;
        Suppliers   _suppliers;

        Suppliers::iterator _nextSupplier4GridRequest{_suppliers.end()};

        const uint16 _amount4Discovered {5};
        const uint16 _amount4NearsSubscription {10};
        const uint16 _amount4GridRequest {100};
        const uint16 _amount4GridRequestOneStep {5};
    };
}
