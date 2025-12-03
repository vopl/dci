// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "aup_legacy_since_2025_04/supplier/catalog.hpp"
#include "aup_legacy_since_2025_04/supplier/storage.hpp"
#include "aup_legacy_since_2025_04/consumer/catalog.hpp"
#include "aup_legacy_since_2025_04/consumer/storage.hpp"

namespace dci::module::ppn::service
{
    class Aup
        : public idl::gen::ppn::service::Aup<>::Opposite
        , public host::module::ServiceBase<Aup>
    {
    public:
        Aup();
        ~Aup();

    private:
        void joined(link::Remote<> r);

    private:
        struct Legacy_since_2025_04
        {
            api_legacy_since_2025_04::SupplierCatalog<>::Opposite _supplierCatalogApi;
            api_legacy_since_2025_04::SupplierStorage<>::Opposite _supplierStorageApi;

            aup_legacy_since_2025_04::supplier::Catalog _supplierCatalog;
            aup_legacy_since_2025_04::supplier::Storage _supplierStorage;

            aup_legacy_since_2025_04::consumer::base::Quota   _consumerQuota;
            aup_legacy_since_2025_04::consumer::Catalog       _consumerCatalog;
            aup_legacy_since_2025_04::consumer::Storage       _consumerStorage;

            Legacy_since_2025_04();
        };
        std::unique_ptr<Legacy_since_2025_04> _legacy_since_2025_04;
    };
}
