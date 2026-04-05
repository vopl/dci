/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/aup/instance/notifiers.hpp>
#include <dci/aup/exception.hpp>
#include "../instance.hpp"

namespace dci::aup::instance::notifiers
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onNewReleaseFound()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onNewReleaseFound();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Set<Oid>> onTargetMostReleases()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetMostReleases();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onTargetCatalogIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetCatalogIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onTargetCatalogComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetCatalogComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onTargetStorageIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetStorageIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onTargetStorageComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetStorageComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Set<Oid>> onBufferMostReleases()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onBufferMostReleases();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onBufferCatalogIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onBufferCatalogIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onBufferCatalogComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onBufferCatalogComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onBufferStorageIncomplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onBufferStorageIncomplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> onBufferStorageComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onBufferStorageComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> onTargetTotallyComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetTotallyComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> onBufferTotallyComplete()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onBufferTotallyComplete();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, applier::Result> onTargetUpdated()
    {
        if(!g_instance) throw aup::Exception{"instance uninitialized"};
        return g_instance->onTargetUpdated();
    }
}
