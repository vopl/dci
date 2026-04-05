/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "aup.hpp"
#include <dci/aup/instance/io.hpp>

namespace dci::module::ppn::service
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Aup::Aup()
        : idl::gen::ppn::service::Aup<>::Opposite{idl::interface::Initializer{}}
    {
        if(!instance::io::instanceInitialized())
        {
            return;
        }

        _legacy_since_2025_04 = std::make_unique<Legacy_since_2025_04>();

        {
            link::Feature<>::Opposite op = *this;

            op->setup() += serviceSol() * [this](link::feature::Service<> srv)
            {
                srv->addPayload(*this);

                srv->joinedByConnect() += serviceSol() * [this](const link::Id&, link::Remote<> r)
                {
                    joined(r);
                };

                srv->joinedByAccept() += serviceSol() * [this](const link::Id&, link::Remote<> r)
                {
                    joined(r);
                };
            };
        }

        {
            link::feature::Payload<>::Opposite op = *this;

            //in ids() -> set<ilid>;
            op->ids() += serviceSol() * []()
            {
                return cmt::readyFuture(Set<idl::interface::Lid>{
                                            api_legacy_since_2025_04::SupplierCatalog<>::lid(),
                                            api_legacy_since_2025_04::SupplierStorage<>::lid()});
            };

            //in getInstance(Id requestorId, Remote requestor, ilid) -> interface;
            op->getInstance() += serviceSol() * [this](const link::Id&, const link::Remote<>&, idl::interface::Lid ilid)
            {
                if(api_legacy_since_2025_04::SupplierCatalog<>::lid() == ilid)
                {
                    return cmt::readyFuture(idl::Interface{_legacy_since_2025_04->_supplierCatalogApi.opposite()});
                }
                if(api_legacy_since_2025_04::SupplierStorage<>::lid() == ilid)
                {
                    return cmt::readyFuture(idl::Interface{_legacy_since_2025_04->_supplierStorageApi.opposite()});
                }

                dbgWarn("crazy link?");

                return cmt::readyFuture<idl::Interface>(exception::buildInstance<api_legacy_since_2025_04::Error>("bad instance ilid requested"));
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Aup::~Aup()
    {
        serviceSol().flush();
        _legacy_since_2025_04.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Aup::joined(link::Remote<> r)
    {
        r->getInstance(api_legacy_since_2025_04::SupplierCatalog<>::lid()).then() += serviceSol() * [this](cmt::Future<idl::Interface> in)
        {
            if(in.resolvedValue())
            {
                _legacy_since_2025_04->_consumerCatalog.involve(in.value());
            }
        };
        r->getInstance(api_legacy_since_2025_04::SupplierStorage<>::lid()).then() += serviceSol() * [this](cmt::Future<idl::Interface> in)
        {
            if(in.resolvedValue())
            {
                _legacy_since_2025_04->_consumerStorage.involve(in.value());
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Aup::Legacy_since_2025_04::Legacy_since_2025_04()
        : _supplierCatalogApi{idl::interface::Initializer{}}
        , _supplierStorageApi{idl::interface::Initializer{}}
        , _supplierCatalog{_supplierCatalogApi}
        , _supplierStorage{_supplierStorageApi}
        , _consumerQuota{100}
        , _consumerCatalog{&_consumerQuota}
        , _consumerStorage{&_consumerQuota}
    {
    }
}
