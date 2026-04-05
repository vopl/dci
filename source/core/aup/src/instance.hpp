/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "instance/importer.hpp"
#include "instance/criteria.hpp"
#include "impl/catalog.hpp"
#include "impl/storage.hpp"
#include <dci/poll/timer.hpp>
#include <dci/aup/applier/result.hpp>
#include <dci/aup/instance/io.hpp>

namespace dci::aup
{
    class Instance;
    extern std::unique_ptr<Instance> g_instance;

    class Instance
    {
    public:
        Instance();
        ~Instance();

    public://setup
        void start(const std::vector<std::string>& args);
        void stop();

        bool targetComplete();
        void updateTarget();
        void collectGarbage();

    public://notifiers
        sbs::Signal<void, Oid>              onNewReleaseFound();

        sbs::Signal<void, Set<Oid>>         onTargetMostReleases();
        sbs::Signal<void, Oid>              onTargetCatalogIncomplete();
        sbs::Signal<void, Oid>              onTargetCatalogComplete();
        sbs::Signal<void, Oid>              onTargetStorageIncomplete();
        sbs::Signal<void, Oid>              onTargetStorageComplete();

        sbs::Signal<void, Set<Oid>>         onBufferMostReleases();
        sbs::Signal<void, Oid>              onBufferCatalogIncomplete();
        sbs::Signal<void, Oid>              onBufferCatalogComplete();
        sbs::Signal<void, Oid>              onBufferStorageIncomplete();
        sbs::Signal<void, Oid>              onBufferStorageComplete();

        sbs::Signal<>                       onTargetTotallyComplete();
        sbs::Signal<>                       onBufferTotallyComplete();

        sbs::Signal<void, applier::Result>  onTargetUpdated();

    public://io
        const Set<Oid>& allReleases() const;

        const Set<Oid>& targetMostReleases() const;
        const Set<Oid>& targetCatalogIncomplete() const;
        const Set<Oid>& targetCatalogComplete() const;
        const Set<Oid>& targetStorageIncomplete() const;
        const Set<Oid>& targetStorageComplete() const;

        const Set<Oid>& bufferMostReleases() const;
        const Set<Oid>& bufferCatalogIncomplete() const;
        const Set<Oid>& bufferCatalogComplete() const;
        const Set<Oid>& bufferStorageIncomplete() const;
        const Set<Oid>& bufferStorageComplete() const;

        bool hasCatalogObject(const Oid& oid);
        bool hasStorageObject(const Oid& oid);

        std::optional<Bytes> getCatalogObject(const Oid& oid);
        std::optional<Bytes> getStorageObject(const Oid& oid, uint32 offset=0, uint32 size=~uint32{});

        instance::io::PutObjectResult putCatalogObject(const Oid& oid, Bytes&& blob);
        instance::io::PutObjectResult putStorageObject(const Oid& oid, Bytes&& blob);
        instance::io::PutObjectResult putStorageObject(const Oid& oid, FILE *f);

    private:
        void loadCatalog();
        void saveCatalog(bool force = true);

    private:
        using Roots = Map<Oid, Set<catalog::File::Kind>>;
        Roots collectRoots4UpdateTarget();
        void collectRequireds(const Oid& oid, Set<Oid>& requiredsCatalog, Set<Oid>& requiredsStorage);

    private:
        struct Index;
        void emitIndexChanges(bool verbose, const Index& prevIndex);

        void updateIndex(bool verbose);
        void updateIndexAfterCatalogObjectComplete(bool verbose, const Oid& oid);
        void updateIndexAfterStorageObjectComplete(bool verbose, const Oid& oid);

        Index buildIndex();
        Set<std::pair<Oid, catalog::ReleasePtr>> collectMostReleases(
                const std::vector<instance::Criteria>& criterias,
                const Set<Oid>& allReleases);
        void collectObjectsIndex(const std::vector<instance::Criteria>& criterias,
                const Oid& oid,
                catalog::Object* o,
                Set<Oid>& catalogIncomplete,
                Set<Oid>& catalogComplete,
                Set<Oid>& storageIncomplete,
                Set<Oid>& storageComplete,
                const Set<Oid>* globalCatalogComplete = nullptr);

    private:
        bool match(const auto* catalogObject, const std::vector<instance::Criteria>& criterias);
        bool match(const auto* catalogObject, bool onlyTargetCriteria);

    private:
        std::filesystem::path           _targetDir;
        std::vector<instance::Criteria> _targetCriterias;
        std::vector<instance::Criteria> _bufferCriterias;

    private:
        instance::Importer  _importer;

    private:
        impl::Catalog   _catalog;
        poll::Timer     _catalogSaveTicker{std::chrono::seconds{1}, false, [this]{saveCatalog(true);}};

    private:
        impl::Storage _storage;

    private:
        sbs::Wire<void, Oid>                _onNewReleaseFound;

        sbs::Wire<void, Set<Oid>>           _onTargetMostReleases;
        sbs::Wire<void, Oid>                _onTargetCatalogIncomplete;
        sbs::Wire<void, Oid>                _onTargetCatalogComplete;
        sbs::Wire<void, Oid>                _onTargetStorageIncomplete;
        sbs::Wire<void, Oid>                _onTargetStorageComplete;

        sbs::Wire<void, Set<Oid>>           _onBufferMostReleases;
        sbs::Wire<void, Oid>                _onBufferCatalogIncomplete;
        sbs::Wire<void, Oid>                _onBufferCatalogComplete;
        sbs::Wire<void, Oid>                _onBufferStorageIncomplete;
        sbs::Wire<void, Oid>                _onBufferStorageComplete;

        sbs::Wire<>                         _onTargetTotallyComplete;
        sbs::Wire<>                         _onBufferTotallyComplete;

        sbs::Wire<void, applier::Result>    _onTargetUpdated;

    private:
        struct Index
        {
            Set<Oid> _allReleases;

            Set<Oid> _targetMostReleases;
            Set<Oid> _targetCatalogIncomplete;
            Set<Oid> _targetCatalogComplete;
            Set<Oid> _targetStorageIncomplete;
            Set<Oid> _targetStorageComplete;

            Set<Oid> _bufferMostReleases;
            Set<Oid> _bufferCatalogIncomplete;
            Set<Oid> _bufferCatalogComplete;
            Set<Oid> _bufferStorageIncomplete;
            Set<Oid> _bufferStorageComplete;

            void reset();
            void swap(Index& other);
        };

        Index _index;
    };
}
