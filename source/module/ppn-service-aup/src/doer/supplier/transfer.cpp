/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "transfer.hpp"

namespace dci::module::ppn::service::aup::doer::supplier
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Transfer::Transfer()
    {
        instance::notifiers::onNewReleaseFound() += _sol * [this](const Oid& oid)
        {
            notifyBlobTransfersAvailable(oid);
        };

        instance::notifiers::onBufferCatalogComplete() += _sol * [this](const Oid& oid)
        {
            notifyBlobTransfersAvailable(oid);
        };

        instance::notifiers::onBufferStorageComplete() += _sol * [this](const Oid& oid)
        {
            notifyBlobTransfersAvailable(oid);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Transfer::~Transfer()
    {
        _sol.flush();
        _oid2BlobTransfers.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Transfer::start(const Oid& oid, api::BlobTransfer<>::Opposite&& api)
    {
        auto iter1 = _oid2BlobTransfers.emplace(oid, BlobTransfers{}).first;
        BlobTransfers& blobTransfers = iter1->second;

        auto [iter2, emplaced] = blobTransfers.emplace(std::piecewise_construct, std::tie(api), std::tuple{});
        BlobTransferState& state = iter2->second;
        if(emplaced)
        {
            api.involvedChanged() += state._sol * [this, iter1, iter2](bool v)
            {
                if(!v)
                {
                    // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer uninvolved");

                    BlobTransfers& blobTransfers = iter1->second;
                    blobTransfers.erase(iter2);
                    if(blobTransfers.empty())
                        _oid2BlobTransfers.erase(iter1);
                }
            };

            api.methods()->getPiece() += state._sol * [iter1, iter2](uint32 offset, uint32 size)
            {
                // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer getPiece offset: " << offset << ", size: " << size);

                auto& [oid, blobTransfers] = *iter1;

                if(std::optional<Bytes> piece = instance::io::getStorageObject(oid, offset, size))
                {
                    // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer getPiece result: " << piece->size() << " bytes (storage)");
                    return cmt::readyFuture(Opt<Bytes>{std::move(piece)});
                }

                if(0 < offset)
                {
                    // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer getPiece result: none");
                    return cmt::readyFuture(Opt<Bytes>{Bytes{}});
                }

                if(std::optional<Bytes> piece = instance::io::getCatalogObject(oid))
                {
                    // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer getPiece result: " << piece->size() << " bytes (catalog)");
                    return cmt::readyFuture(Opt<Bytes>{std::move(piece)});
                }

                auto& [api, state] = *iter2;
                state._waitAvailability = true;

                // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer getPiece result: wait availability");
                return cmt::readyFuture(Opt<Bytes>{});
            };
        }

        api.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Transfer::notifyBlobTransfersAvailable(const Oid& oid)
    {
        auto iter1 = _oid2BlobTransfers.find(oid);
        if(_oid2BlobTransfers.end() == iter1)
            return;

        BlobTransfers& blobTransfers = iter1->second;
        for(auto& [api, state] : blobTransfers)
        {
            if(state._waitAvailability)
            {
                state._waitAvailability = false;

                // LOGD("supplier for " << utils::b2h(iter1->first.data(), 5) << " blobTransfer notify available ");
                api->available();
            }
        }
    }
}
