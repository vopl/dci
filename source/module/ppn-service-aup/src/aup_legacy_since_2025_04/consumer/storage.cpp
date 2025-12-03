// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "storage.hpp"
#include "../../aup.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::Storage(base::Quota* quota)
        : Base{"storage consumer", quota}
    {
        dci::aup::instance::notifiers::onTargetStorageIncomplete() += _sbsOwner * [this](const Oid& oid)
        {
            addIncomplete(oid, _prioTarget);
        };

        dci::aup::instance::notifiers::onBufferStorageIncomplete() += _sbsOwner * [this](const Oid& oid)
        {
            addIncomplete(oid, _prioBuffer);
        };

        for(const Oid& oid : dci::aup::instance::io::targetStorageIncomplete())
        {
            addIncomplete(oid, _prioTarget);
        }

        for(const Oid& oid : dci::aup::instance::io::bufferStorageIncomplete())
        {
            addIncomplete(oid, _prioBuffer);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::~Storage()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::onComplete(const Oid& oid, base::RecvBuffer& recvBuffer)
    {
        dci::aup::instance::io::PutObjectResult por = recvBuffer.hasFile() ?
            dci::aup::instance::io::putStorageObject(oid, recvBuffer.getFile()):
            dci::aup::instance::io::putStorageObject(oid, recvBuffer.detachBytes());

        switch(por)
        {
        case dci::aup::instance::io::PutObjectResult::ok:
            return true;
        case dci::aup::instance::io::PutObjectResult::unwanted:
            LOGW(_name<<": unwanted blob received: "<<utils::b2h(oid));
            return true;
        case dci::aup::instance::io::PutObjectResult::corrupted:
            LOGW(_name<<": bad blob received: "<<utils::b2h(oid));
            return false;
        }

        return false;
    }
}
