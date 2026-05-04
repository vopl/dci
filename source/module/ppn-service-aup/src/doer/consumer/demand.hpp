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
#include "destiny.hpp"

namespace dci::module::ppn::service::aup::doer::consumer
{
    struct Demand
    {
        Oid         _oid{};
        int         _priority{};

        std::tuple<int, const Oid&> order() const
        {
            return std::tuple<int, const Oid&>{-_priority, _oid};
        }

        std::size_t _supplierBound{};

        mutable Destiny             _destiny{};
        mutable cmt::task::Owner    _tol;
        mutable cmt::Notifier       _workerAwaker;


        struct DelayedTransfer
        {
            api::BlobTransfer<> _api;

            mutable link::Id    _supplierRid;
            mutable std::size_t _supplierNum;
            mutable sbs::Owner  _sol;

            bool operator<(const DelayedTransfer&                rhs) const { return _api < rhs._api; }
            bool operator<(const api::BlobTransfer<>&            rhs) const { return _api < rhs;      }
            bool operator<(const idl::interface::Generic<false>& rhs) const { return _api < rhs;      }

            friend bool operator<(const api::BlobTransfer<>&            lhs, const DelayedTransfer& rhs) { return lhs < rhs._api; }
            friend bool operator<(const idl::interface::Generic<false>& lhs, const DelayedTransfer& rhs) { return lhs < rhs._api.weak(); }
        };

        mutable std::set<DelayedTransfer, std::less<void>>
                                    _delayedTransfersEmpty;
        mutable std::set<DelayedTransfer, std::less<void>>
                                    _delayedTransfersRevived;
    };
}
