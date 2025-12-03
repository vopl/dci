// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "base.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::supplier
{
    class Catalog
        : public Base
    {
    public:
        Catalog(api_legacy_since_2025_04::SupplierCatalog<>::Opposite sc);
        ~Catalog() override;

    private:
        Bytes getPiece(const Oid& oid, uint32 offset, uint32 size) override;
        api_legacy_since_2025_04::BlobStatus getStatus(const Oid& oid) override;

    private:
        api_legacy_since_2025_04::SupplierCatalog<>::Opposite   _sc;
        Set<Oid>                                                _releases;
    };
}
