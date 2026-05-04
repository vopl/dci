/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "supplier.hpp"

namespace dci::module::ppn::service::aup::doer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Supplier::Supplier(const api::Supplier<>::Opposite& api)
        : _api{api}
    {
        // out newRelease(Oid);
        instance::notifiers::onNewReleaseFound() += _sol * [this](const Oid& oid)
        {
            _api->newRelease(oid);
        };

        // in getReleases() -> set<Oid>;
        _api.methods()->getReleases() += _sol * []
        {
            return cmt::readyFuture(instance::io::allReleases());
        };

        // in startBlobTransfer(Oid, BlobTransfer::Opposite);
        _api.methods()->startBlobTransfer() += _sol * [this](const Oid& oid, api::BlobTransfer<>::Opposite&& blobTransfer)
        {
            // LOGD("supplier for " << utils::b2h(oid.data(), 5) << " startBlobTransfer");
            _transfer.start(oid, std::move(blobTransfer));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Supplier::~Supplier()
    {
        _sol.flush();
    }
}
