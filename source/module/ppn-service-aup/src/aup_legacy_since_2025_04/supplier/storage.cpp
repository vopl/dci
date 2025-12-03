// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "storage.hpp"
#include "../../aup.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::supplier
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::Storage(api_legacy_since_2025_04::SupplierStorage<>::Opposite ss)
        : _ss{std::move(ss)}
    {
        //in startBlobTransfer(Oid, BlobTransfer);
        _ss->startBlobTransfer() += _sbsOwner * [this](const Oid& oid, api_legacy_since_2025_04::BlobTransfer<>::Opposite&& bt)
        {
            return Base::startOne(oid, std::move(bt));
        };

        //out statusChanged(BlobStatus);
        instance::notifiers::onBufferStorageIncomplete() += _sbsOwner * [this](const Oid& oid)
        {
            auto iter = _transfers.find(oid);
            if(_transfers.end() != iter)
            {
                iter->second.updateStatus();
            }
        };

        instance::notifiers::onBufferStorageComplete() += _sbsOwner * [this](const Oid& oid)
        {
            auto iter = _transfers.find(oid);
            if(_transfers.end() != iter)
            {
                iter->second.updateStatus();
            }
        };

        instance::notifiers::onBufferMostReleases() += _sbsOwner * [this](const Set<Oid>&)
        {
            for(auto iter {_transfers.begin()}; iter!=_transfers.end(); ++iter)
            {
                iter->second.updateStatus();
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::~Storage()
    {
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Storage::getPiece(const Oid& oid, uint32 offset, uint32 size)
    {
        auto obj = instance::io::getStorageObject(oid, offset, size);
        if(!obj)
        {
            return Bytes{};
        }

        return std::move(*obj);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    api_legacy_since_2025_04::BlobStatus Storage::getStatus(const Oid& oid)
    {
        if(instance::io::bufferStorageComplete().count(oid))
        {
            return api_legacy_since_2025_04::BlobStatus::present;
        }

        if(instance::io::bufferStorageIncomplete().count(oid))
        {
            return api_legacy_since_2025_04::BlobStatus::missingAndWanted;
        }

        return api_legacy_since_2025_04::BlobStatus::missingAndUnwanted;
    }

}

