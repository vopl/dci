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
#include "consumer/destiny.hpp"
#include "consumer/demand.hpp"
#include "consumer/supplier.hpp"
#include "consumer/recvBuffer.hpp"

namespace dci::module::ppn::service::aup::doer
{
    class Consumer
    {
    public:
        Consumer();
        ~Consumer();

        void joined(const link::Id& rid, api::Supplier<>&& api);

    private:
        using Destiny = consumer::Destiny;
        using Demand = consumer::Demand;
        using Supplier = consumer::Supplier;

    private:
        void addIncomplete(const Oid& oid, Destiny destiny);
        void fixComplete(const Oid& oid, Destiny destiny);

        void fireWorker();
        void worker(const Demand& demand);
        void worker(const Demand& demand, const link::Id& rid, std::size_t num, api::BlobTransfer<> api, consumer::RecvBuffer& recvBuffer);

    private:
        sbs::Owner          _sol;

    private:
        using SupplierByRid    = bmi::member<Supplier, link::Id,    &Supplier::_rid   >;
        using SupplierByNumber = bmi::member<Supplier, std::size_t, &Supplier::_number>;

        using Suppliers = bmi::multi_index_container
        <
            Supplier,
            bmi::indexed_by
            <
                bmi::ordered_unique <bmi::tag<SupplierByRid>,    SupplierByRid>,
                bmi::ordered_unique <bmi::tag<SupplierByNumber>, SupplierByNumber>
            >
        >;

        Suppliers   _suppliers;
        std::size_t _supplierNumberGen{1};

    private:
        using DemandByOid           = bmi::member       <Demand, Oid,          &Demand::_oid           >;
        using DemandBySupplierBound = bmi::member       <Demand, std::size_t,  &Demand::_supplierBound >;

        using Demands = bmi::multi_index_container
        <
            Demand,
            bmi::indexed_by
            <
                bmi::ordered_unique     <bmi::tag<DemandByOid>,             DemandByOid>,
                bmi::ordered_non_unique <bmi::tag<DemandBySupplierBound>,   DemandBySupplierBound>
            >
        >;
        Demands _demands;
        Demands _demandsProcessing;
        static constexpr std::size_t    _maxWorkersCount{10};
        static constexpr std::size_t    _granulaSize{1024 * 1024 * 1};
    };
}
