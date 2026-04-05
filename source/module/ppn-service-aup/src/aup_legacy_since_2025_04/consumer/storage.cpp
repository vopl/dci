/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
