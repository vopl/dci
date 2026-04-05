/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/aup/instance/io.hpp>
#include <dci/aup/exception.hpp>
#include "../instance.hpp"

namespace dci::aup::instance::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool instanceInitialized()
    {
        return !!g_instance;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& allReleases()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->allReleases();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& targetMostReleases()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->targetMostReleases();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& targetCatalogIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->targetCatalogIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& targetCatalogComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->targetCatalogComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& targetStorageIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->targetStorageIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& targetStorageComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->targetStorageComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& bufferMostReleases()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->bufferMostReleases();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& bufferCatalogIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->bufferCatalogIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& bufferCatalogComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->bufferCatalogComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& bufferStorageIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->bufferStorageIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& bufferStorageComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->bufferStorageComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool hasCatalogObject(const Oid& oid)
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->hasCatalogObject(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool hasStorageObject(const Oid& oid)
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->hasStorageObject(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> getCatalogObject(const Oid& oid)
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->getCatalogObject(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> getStorageObject(const Oid& oid, uint32 offset, uint32 size)
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->getStorageObject(oid, offset, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PutObjectResult putCatalogObject(const Oid& oid, Bytes&& blob)
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->putCatalogObject(oid, std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PutObjectResult putStorageObject(const Oid& oid, Bytes&& blob)
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->putStorageObject(oid, std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    PutObjectResult putStorageObject(const Oid& oid, std::FILE* f)
    {
        dbgAssert(f);
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->putStorageObject(oid, f);
    }
}
