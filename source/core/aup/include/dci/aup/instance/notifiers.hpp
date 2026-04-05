/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../api.hpp"
#include "../oid.hpp"
#include <dci/sbs/signal.hpp>
#include <dci/aup/applier/result.hpp>

namespace dci::aup::instance::notifiers
{
    API_DCI_AUP sbs::Signal<void, Oid>              onNewReleaseFound();

    API_DCI_AUP sbs::Signal<void, Set<Oid>>         onTargetMostReleases();
    API_DCI_AUP sbs::Signal<void, Oid>              onTargetCatalogIncomplete();
    API_DCI_AUP sbs::Signal<void, Oid>              onTargetCatalogComplete();
    API_DCI_AUP sbs::Signal<void, Oid>              onTargetStorageIncomplete();
    API_DCI_AUP sbs::Signal<void, Oid>              onTargetStorageComplete();

    API_DCI_AUP sbs::Signal<void, Set<Oid>>         onBufferMostReleases();
    API_DCI_AUP sbs::Signal<void, Oid>              onBufferCatalogIncomplete();
    API_DCI_AUP sbs::Signal<void, Oid>              onBufferCatalogComplete();
    API_DCI_AUP sbs::Signal<void, Oid>              onBufferStorageIncomplete();
    API_DCI_AUP sbs::Signal<void, Oid>              onBufferStorageComplete();

    API_DCI_AUP sbs::Signal<>                       onTargetTotallyComplete();
    API_DCI_AUP sbs::Signal<>                       onBufferTotallyComplete();

    API_DCI_AUP sbs::Signal<void, applier::Result>  onTargetUpdated();
}
