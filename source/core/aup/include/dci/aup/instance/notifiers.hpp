// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
