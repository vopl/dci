// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "base.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    class Catalog
        : public Base
    {
    public:
        Catalog(base::Quota* quota);
        ~Catalog() override;

        void involve(api_legacy_since_2025_04::SupplierCatalog<>&& remoteApi);

    private:
        bool onComplete(const Oid& oid, base::RecvBuffer& recvBuffer) override;

    private:
        static constexpr int _prioRelease = 21;
        static constexpr int _prioTarget = 20;
        static constexpr int _prioBuffer = 19;

        void releaseSupplied(const Oid& oid);

    };
}
