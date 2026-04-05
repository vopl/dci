/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "catalog.hpp"
#include "../../aup.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::Catalog(base::Quota* quota)
        : Base{"catalog consumer", quota}
    {
        {
            std::error_code ec;
            std::filesystem::path tmpName = std::filesystem::temp_directory_path(ec);
            if(ec)
            {
                LOGE("consumer unable to obtain temporary directory name: "<<ec);
            }
            else
            {
                std::filesystem::directory_iterator directoryIterator{tmpName, ec};
                if(ec)
                {
                    LOGE("consumer unable to enumerate directory `"<<tmpName<<"`: "<<ec);
                }
                else
                {
                    for(const std::filesystem::directory_entry& entry : directoryIterator)
                    {
                        if(entry.path().stem().string().starts_with("dci-ppn-aup"))
                        {
                            if(entry.is_regular_file())
                            {
                                std::filesystem::remove(entry.path(), ec);
                                if(ec)
                                {
                                    LOGE("consumer unable to remove file `"<<entry.path()<<"`: "<<ec);
                                }
                            }
                        }
                    }
                }
            }
        }

        dci::aup::instance::notifiers::onTargetCatalogIncomplete() += _sbsOwner * [this](const Oid& oid)
        {
            addIncomplete(oid, _prioTarget);
        };

        dci::aup::instance::notifiers::onBufferCatalogIncomplete() += _sbsOwner * [this](const Oid& oid)
        {
            addIncomplete(oid, _prioBuffer);
        };

        for(const Oid& oid : dci::aup::instance::io::targetCatalogIncomplete())
        {
            addIncomplete(oid, _prioTarget);
        }

        for(const Oid& oid : dci::aup::instance::io::bufferCatalogIncomplete())
        {
            addIncomplete(oid, _prioBuffer);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::~Catalog()
    {
        _sbsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::involve(api_legacy_since_2025_04::SupplierCatalog<>&& remoteApi)
    {
        base::Remote* r = Base::involve(base::remote::Api{remoteApi});

        remoteApi->getReleases().then() += r->sol() * [this](auto in)
        {
            if(in.resolvedValue())
            {
                for(const Oid& oid : in.value())
                {
                    releaseSupplied(oid);
                }
            }
        };

        remoteApi->newRelease() += r->sol() * [this](const Oid& oid)
        {
            releaseSupplied(oid);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Catalog::onComplete(const Oid& oid, base::RecvBuffer& recvBuffer)
    {
        switch(dci::aup::instance::io::putCatalogObject(oid, recvBuffer.detachBytes()))
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

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::releaseSupplied(const Oid& oid)
    {
        if(!dci::aup::instance::io::allReleases().count(oid))
        {
            if(addIncomplete(oid, _prioRelease))
            {
                LOGI(_name<<": some release found: "<<dci::utils::b2h(oid));
            }
        }
    }

}
