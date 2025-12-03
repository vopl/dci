// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::supplier
{
    class Base
    {
    public:
        Base();
        virtual ~Base();

        void startOne(const Oid& oid, api_legacy_since_2025_04::BlobTransfer<>::Opposite&& bt);

    protected:
        virtual Bytes getPiece(const Oid& oid, uint32 offset, uint32 size) = 0;
        virtual api_legacy_since_2025_04::BlobStatus getStatus(const Oid& oid) = 0;

    private:
        void transferBecomesEmpty(const Oid& oid);

    protected:
        sbs::Owner  _sbsOwner;

    protected:
        class Transfer
        {
        public:
            Transfer(Base* base, const Oid& oid);
            ~Transfer();

            void startOne(api_legacy_since_2025_04::BlobTransfer<>::Opposite&& bt);
            void updateStatus();

        private:
            Base *                                                  _base;
            const Oid                                               _oid;
            api_legacy_since_2025_04::BlobStatus                    _status{api_legacy_since_2025_04::BlobStatus::present};
            Set<api_legacy_since_2025_04::BlobTransfer<>::Opposite> _ifaces;

            sbs::Owner                                              _sbsOwner;
        };

        Map<Oid, Transfer> _transfers;
    };
}
