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
