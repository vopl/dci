/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "instance.hpp"
#include <dci/aup/exception.hpp>
#include <dci/aup/applier/task.hpp>
#include <dci/aup/applier/result.hpp>
#include <dci/aup/catalog/identify.hpp>
#include <dci/logger.hpp>
#include <dci/crypto/ed25519.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/h2b.hpp>
#include <dci/utils/time.hpp>

#include "dci/integration/info.hpp"
#include "impl/applier.hpp"
#include "impl/catalog/serializeObject.hpp"
#include "impl/catalog/deserializeObject.hpp"

#include <filesystem>

namespace std
{
    template <class Iter>
    Iter begin(const std::pair<Iter, Iter>& range)
    {
        return range.first;
    }

    template <class Iter>
    Iter end(const std::pair<Iter, Iter>& range)
    {
        return range.second;
    }
}

namespace dci::aup
{
    namespace fs = std::filesystem;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        bool checkSignature(const catalog::Release* r)
        {
            catalog::Release r0 = *r;
            r0._signature.fill(0);
            Oid releaseHash = catalog::identify(&r0);
            return crypto::ed25519::verify(releaseHash.data(), releaseHash.size(),
                                           r->_vendorSign.data(),
                                           r->_signature.data());
        }

        void dump(const Oid& oid, const catalog::Release* r)
        {
            LOGI("release                : "<<utils::b2h(oid));

            LOGI("srcBranch              : "<<r->_srcBranch);
            LOGI("srcRevision            : "<<r->_srcRevision);

            LOGI("srcMoment              : "<<utils::time::moment2DatetimeTz(r->_srcMoment));
            LOGI("(version)              : "<<integration::info::version(r->_srcBranch, r->_srcRevision, r->_srcMoment));
            LOGI("platformOs             : "<<r->_platformOs);
            LOGI("platformArch           : "<<r->_platformArch);
            LOGI("compiler               : "<<r->_compiler);
            LOGI("compilerVersion        : "<<r->_compilerVersion);
            LOGI("compilerOptimization   : "<<r->_compilerOptimization);

            LOGI("stability              : "<<r->_stability);

            LOGI("vendor                 : "<<r->_vendor);
            LOGI("vendorSign             : "<<utils::b2h(r->_vendorSign));
            LOGI("signature              : "<<utils::b2h(r->_signature));

//            for(const auto& e : r->_dependencies)
//            {
//                LOGI("dep                    : "<<utils::b2h(e));
//            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::unique_ptr<Instance> g_instance {};

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::Instance()
    {
//        _importer.emitStart() += [this]()
//        {
//        });

        _importer.emitData() += [this](impl::Catalog* c, impl::Storage* s)
        {
            _catalog.import(c, [](const Oid&/*oid*/, const catalog::ObjectPtr& object)
            {
                if(catalog::Object::Type::release == object->type())
                {
                    return checkSignature(catalog::objectPtrCast<catalog::Release>(object.get()));
                }

                return true;
            });

            saveCatalog();

            _storage.import(s);
        };

        _importer.emitFinish() += [this]()
        {
            updateIndex(true);
            collectGarbage();

            saveCatalog();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::~Instance()
    {
        stop();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::start(const std::vector<std::string>& args)
    {
        using namespace boost::property_tree;

        try
        {
            ptree c = config::parse(args);

            _targetDir = c.get("targetDir", "..");
            _storage.reset(c.get("stateDir", "../var/aup"), true);

            for(const auto& kv : c.equal_range("target"))
            {
                instance::Criteria criteria = instance::Criteria::parse(kv.second);
                _targetCriterias.push_back(criteria);
                _bufferCriterias.emplace_back(std::move(criteria));
            }

            for(const auto& kv : c.equal_range("buffer"))
            {
                _bufferCriterias.push_back(instance::Criteria::parse(kv.second));
            }

            loadCatalog();

            fs::path importDir = c.get("importDir", "");
            if(!importDir.empty() && fs::is_directory(importDir))
            {
                _importer.setup(importDir);
                _importer.start();
            }
        }
        catch(...)
        {
            stop();
            std::throw_with_nested(Exception{"unable to start instance"});
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::stop()
    {
        saveCatalog(false);

        _importer.stop();

        _targetDir.clear();
        _targetCriterias.clear();
        _bufferCriterias.clear();

        _catalog.reset();
        _catalogSaveTicker.stop();

        _storage.reset();

        _index.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Instance::targetComplete()
    {
        return _index._targetCatalogIncomplete.empty() &&
               _index._targetStorageIncomplete.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::updateTarget()
    {
        applier::Result res{};

        {
            impl::Applier a;
            a.addCatalog(&_catalog);
            a.addStorage(&_storage);

            for(const auto&[k, v] : collectRoots4UpdateTarget())
            {
                a.addRoot(k, v);
            }

            res = a.process(_targetDir.string(), static_cast<applier::Task>(
                                applier::tVerbose |
                                applier::tRemoveWrongs |
                                applier::tEmplaceMissings |
                                applier::tEmplaceChanges |
                                applier::tRemoveExtra));
        }

        _onTargetUpdated.in(res);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::collectGarbage()
    {
        Set<Oid> requiredsCatalog, requiredsStorage;

        for(const Oid& oid : _index._allReleases)
        {
            collectRequireds(oid, requiredsCatalog, requiredsStorage);
        }

        uint32 dropped = _storage.dropOthersThan(requiredsStorage);
        if(dropped)
        {
            LOGI("drop "<<dropped<<" garbage object(s) from storage");
        }

        dropped = _catalog.dropOthersThan(requiredsCatalog);
        if(dropped)
        {
            LOGI("drop "<<dropped<<" garbage object(s) from catalog");
            _catalogSaveTicker.start();
        }

        updateIndex(true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onNewReleaseFound()
    {
        return _onNewReleaseFound.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Set<Oid>> Instance::onTargetMostReleases()
    {
        return _onTargetMostReleases.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onTargetCatalogIncomplete()
    {
        return _onTargetCatalogIncomplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onTargetCatalogComplete()
    {
        return _onTargetCatalogComplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onTargetStorageIncomplete()
    {
        return _onTargetStorageIncomplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onTargetStorageComplete()
    {
        return _onTargetStorageComplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Set<Oid>> Instance::onBufferMostReleases()
    {
        return _onBufferMostReleases.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onBufferCatalogIncomplete()
    {
        return _onBufferCatalogIncomplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onBufferCatalogComplete()
    {
        return _onBufferCatalogComplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onBufferStorageIncomplete()
    {
        return _onBufferStorageIncomplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, Oid> Instance::onBufferStorageComplete()
    {
        return _onBufferStorageComplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Instance::onTargetTotallyComplete()
    {
        return _onTargetTotallyComplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Instance::onBufferTotallyComplete()
    {
        return _onBufferTotallyComplete.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<void, applier::Result> Instance::onTargetUpdated()
    {
        return _onTargetUpdated.out();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::allReleases() const
    {
        return _index._allReleases;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::targetMostReleases() const
    {
        return _index._targetMostReleases;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::targetCatalogIncomplete() const
    {
        return _index._targetCatalogIncomplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::targetCatalogComplete() const
    {
        return _index._targetCatalogComplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::targetStorageIncomplete() const
    {
        return _index._targetStorageIncomplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::targetStorageComplete() const
    {
        return _index._targetStorageComplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::bufferMostReleases() const
    {
        return _index._bufferMostReleases;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::bufferCatalogIncomplete() const
    {
        return _index._bufferCatalogIncomplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::bufferCatalogComplete() const
    {
        return _index._bufferCatalogComplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::bufferStorageIncomplete() const
    {
        return _index._bufferStorageIncomplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<Oid>& Instance::bufferStorageComplete() const
    {
        return _index._bufferStorageComplete;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Instance::hasCatalogObject(const Oid& oid)
    {
        return _catalog.has(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Instance::hasStorageObject(const Oid& oid)
    {
        return _storage.has(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Instance::getCatalogObject(const Oid& oid)
    {
        catalog::ObjectPtr object = _catalog.get(oid);

        if(!object)
        {
            return {};
        }

        return impl::catalog::serializeObject(object);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Instance::getStorageObject(const Oid& oid, uint32 offset, uint32 size)
    {
        return _storage.get(oid, offset, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    instance::io::PutObjectResult Instance::putCatalogObject(const Oid& oid, Bytes&& blob)
    {
        if(catalog::identify(blob) != oid)
        {
            return instance::io::PutObjectResult::corrupted;
        }

        catalog::ObjectPtr o = impl::catalog::deserializeObject(std::move(blob));
        if(!o)
        {
            return instance::io::PutObjectResult::corrupted;
        }

        bool isRelease = false;
        if(catalog::Object::Type::release == o->type())
        {
            isRelease = true;

            if(_index._allReleases.count(oid))
            {
                //already
                return instance::io::PutObjectResult::unwanted;
            }

            catalog::Release* r = catalog::objectPtrCast<catalog::Release>(o.get());

            if(!checkSignature(r))
            {
                //bad signature
                LOGW("bad release found, signature mismatch: "<<utils::b2h(r->_vendorSign)<<", "<<utils::b2h(r->_signature));
                return instance::io::PutObjectResult::corrupted;
            }
        }
        else
        {
            if(!_index._targetCatalogIncomplete.count(oid) && !_index._bufferCatalogIncomplete.count(oid))
            {
                //unwanted
                return instance::io::PutObjectResult::unwanted;
            }
        }

        _catalog.put(std::move(o));
        _catalogSaveTicker.start();

        if(isRelease)
        {
            updateIndex(true);
        }
        else
        {
            updateIndexAfterCatalogObjectComplete(true, oid);
        }

        return instance::io::PutObjectResult::ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    instance::io::PutObjectResult Instance::putStorageObject(const Oid& oid, Bytes&& blob)
    {
        if(!_index._targetStorageIncomplete.count(oid) && !_index._bufferStorageIncomplete.count(oid))
        {
            return instance::io::PutObjectResult::unwanted;
        }

        if(catalog::identify(blob) != oid)
        {
            return instance::io::PutObjectResult::corrupted;
        }

        _storage.put(oid, std::move(blob));
        updateIndexAfterStorageObjectComplete(true, oid);

        return instance::io::PutObjectResult::ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    instance::io::PutObjectResult Instance::putStorageObject(const Oid& oid, std::FILE* f)
    {
        if(!_index._targetStorageIncomplete.count(oid) && !_index._bufferStorageIncomplete.count(oid))
        {
            return instance::io::PutObjectResult::unwanted;
        }

        dbgAssert(f);
        if(catalog::identify(f) != oid)
        {
            return instance::io::PutObjectResult::corrupted;
        }

        _storage.put(oid, f);
        updateIndexAfterStorageObjectComplete(true, oid);

        return instance::io::PutObjectResult::ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::loadCatalog()
    {
        try
        {
            std::optional<Bytes> blob = _storage.get("catalog");
            if(blob)
            {
                _catalog.deserialize(std::move(*blob));

                Set<Oid> badReleases;
                for(const Oid& oid: _catalog.enumerate(catalog::Object::Type::release))
                {
                    catalog::ReleasePtr r = catalog::objectPtrCast<catalog::Release>(_catalog.get(oid));

                    if(!checkSignature(r.get()))
                    {
                        LOGW("bad release found, signature mismatch: "<<utils::b2h(r->_vendorSign)<<", "<<utils::b2h(r->_signature));
                        badReleases.insert(oid);
                    }
                }

                if(!badReleases.empty())
                {
                    for(const Oid& oid: badReleases)
                    {
                        _catalog.del(oid);
                    }
                    _catalogSaveTicker.start();
                }

                updateIndex(false);
            }
        }
        catch(...)
        {
            LOGW("unable to load catalog: "<<dci::exception::toString(std::current_exception()));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::saveCatalog(bool force)
    {
        if(!force && !_catalogSaveTicker.started())
        {
            return;
        }

        try
        {
            _catalogSaveTicker.stop();
            _storage.put("catalog", _catalog.serialize());
        }
        catch(...)
        {
            LOGW("unable to save catalog: "<<dci::exception::toString(std::current_exception()));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::Roots Instance::collectRoots4UpdateTarget()
    {
        Roots res;

        for(const Oid& releaseOid : _index._targetMostReleases)
        {
            catalog::ReleasePtr r = catalog::objectPtrCast<catalog::Release>(_catalog.get(releaseOid));

            LOGI("releases for update target:");
            dump(releaseOid, r.get());

            for(const Oid& unitOid : r->_dependencies)
            {
                catalog::UnitPtr u = catalog::objectPtrCast<catalog::Unit>(_catalog.get(unitOid));
                if(!u)
                {
                    continue;
                }

                for(const instance::Criteria& c : _targetCriterias)
                {
                    if(c.match(u.get()))
                    {
                        auto& rootValue = res[unitOid];

                        constexpr catalog::File::Kind allFKinds[] =
                        {
                            catalog::File::Kind::null,
                            catalog::File::Kind::runtime,
                            catalog::File::Kind::resource,
                            catalog::File::Kind::rdep,
                            catalog::File::Kind::test,
                            catalog::File::Kind::debug,
                            catalog::File::Kind::bdep,
                            catalog::File::Kind::include,
                            catalog::File::Kind::idl,
                            catalog::File::Kind::cmm,
                            catalog::File::Kind::src,
                        };
                        for(catalog::File::Kind fkind : allFKinds)
                        {
                            if(instance::criteria::MatchResult::allow == c._fileKind.match(fkind))
                            {
                                rootValue.insert(fkind);
                            }
                        }
                    }
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::collectRequireds(const Oid& oid, Set<Oid>& requiredsCatalog, Set<Oid>& requiredsStorage)
    {
        catalog::ObjectPtr o = _catalog.get(oid);

        if(!o)
        {
            requiredsCatalog.insert(oid);
            return;
        }

        switch(o->type())
        {
        case catalog::Object::Type::release:
            {
                catalog::Release* r = catalog::objectPtrCast<catalog::Release>(o.get());
                if(!match(r, false))
                {
                    requiredsCatalog.insert(oid);
                    return;
                }
            }
            break;
        case catalog::Object::Type::unit:
            {
                catalog::Unit* u = catalog::objectPtrCast<catalog::Unit>(o.get());
                if(!match(u, false))
                {
                    requiredsCatalog.insert(oid);
                    return;
                }
            }
            break;
        case catalog::Object::Type::file:
            {
                catalog::File* f = catalog::objectPtrCast<catalog::File>(o.get());
                if(!match(f, false))
                {
                    requiredsCatalog.insert(oid);
                    return;
                }
                requiredsStorage.insert(f->_content);
            }
            break;
        default:
            dbgWarn("internal error");
            break;
        }

        if(requiredsCatalog.insert(oid).second)
        {
            for(const Oid& dep : o->_dependencies)
            {
                collectRequireds(dep, requiredsCatalog, requiredsStorage);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::emitIndexChanges(bool verbose, const Index& prevIndex)
    {
        //sbs::Wire<void, Oid> _onNewReleaseFound;
        for(const Oid& oid : _index._allReleases)
        {
            if(!prevIndex._allReleases.count(oid))
            {
                if(verbose)
                {
                    catalog::ReleasePtr r = catalog::objectPtrCast<catalog::Release>(_catalog.get(oid));

                    LOGI("new release found:");
                    dump(oid, r.get());
                }

                _onNewReleaseFound.in(oid);
            }
        }

        bool someChanged4MostTargetReleases = false;
        bool someChanged4MostBufferReleases = false;

        //sbs::Wire<void, Set<Oid>>    _onTargetMostReleases;
        if(_index._targetMostReleases != prevIndex._targetMostReleases)
        {
            if(verbose)
            {
                LOGI("target most releases changed");
            }

            someChanged4MostTargetReleases = true;
            _onTargetMostReleases.in(_index._targetMostReleases);
        }

        //sbs::Wire<void, Oid>              _onTargetCatalogIncomplete;
        for(const Oid& oid : _index._targetCatalogIncomplete)
        {
            if(!prevIndex._targetCatalogIncomplete.count(oid))
            {
                _onTargetCatalogIncomplete.in(oid);
            }
        }

        //sbs::Wire<void, Oid>              _onTargetCatalogComplete;
        for(const Oid& oid : _index._targetCatalogComplete)
        {
            if(!prevIndex._targetCatalogComplete.count(oid))
            {
                someChanged4MostTargetReleases = true;
                _onTargetCatalogComplete.in(oid);
            }
        }

        //sbs::Wire<void, Oid>              _onTargetStorageIncomplete;
        for(const Oid& oid : _index._targetStorageIncomplete)
        {
            if(!prevIndex._targetStorageIncomplete.count(oid))
            {
                _onTargetStorageIncomplete.in(oid);
            }
        }

        //sbs::Wire<void, Oid>              _onTargetStorageComplete;
        for(const Oid& oid : _index._targetStorageComplete)
        {
            if(!prevIndex._targetStorageComplete.count(oid))
            {
                someChanged4MostTargetReleases = true;
                _onTargetStorageComplete.in(oid);
            }
        }

        //sbs::Wire<void, Set<Oid>>    _onBufferMostReleases;
        if(_index._bufferMostReleases != prevIndex._bufferMostReleases)
        {
            someChanged4MostBufferReleases = true;
            _onBufferMostReleases.in(_index._bufferMostReleases);
        }

        //sbs::Wire<void, Oid>              _onBufferCatalogIncomplete;
        for(const Oid& oid : _index._bufferCatalogIncomplete)
        {
            if(!prevIndex._bufferCatalogIncomplete.count(oid))
            {
                _onBufferCatalogIncomplete.in(oid);
            }
        }

        //sbs::Wire<void, Oid>              _onBufferCatalogComplete;
        for(const Oid& oid : _index._bufferCatalogComplete)
        {
            if(!prevIndex._bufferCatalogComplete.count(oid))
            {
                someChanged4MostBufferReleases = true;
                _onBufferCatalogComplete.in(oid);
            }
        }

        //sbs::Wire<void, Oid>              _onBufferStorageIncomplete;
        for(const Oid& oid : _index._bufferStorageIncomplete)
        {
            if(!prevIndex._bufferStorageIncomplete.count(oid))
            {
                _onBufferStorageIncomplete.in(oid);
            }
        }

        //sbs::Wire<void, Oid>              _onBufferStorageComplete;
        for(const Oid& oid : _index._bufferStorageComplete)
        {
            if(!prevIndex._bufferStorageComplete.count(oid))
            {
                someChanged4MostBufferReleases = true;
                _onBufferStorageComplete.in(oid);
            }
        }

        //sbs::Wire<> _onTargetTotallyComplete;
        if(someChanged4MostTargetReleases &&
           _index._targetCatalogIncomplete.empty() &&
           _index._targetStorageIncomplete.empty())
        {
            if(verbose)
            {
                LOGI("target totally complete");
            }

            saveCatalog(false);
            _onTargetTotallyComplete.in();
        }

        //sbs::Wire<> _onBufferTotallyComplete;
        if(someChanged4MostBufferReleases &&
           _index._bufferCatalogIncomplete.empty() &&
           _index._bufferStorageIncomplete.empty())
        {
            if(verbose)
            {
                LOGI("buffer totally complete");
            }

            _onBufferTotallyComplete.in();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::updateIndex(bool verbose)
    {
        Index s2 = buildIndex();
        s2.swap(_index);

        emitIndexChanges(verbose, s2);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::updateIndexAfterCatalogObjectComplete(bool verbose, const Oid& oid)
    {
        catalog::ObjectPtr o = _catalog.get(oid);
        if(!o)
        {
            return;
        }

        // target delta
        Set<Oid> targetCatalogIncomplete;
        Set<Oid> targetCatalogComplete;
        Set<Oid> targetStorageIncomplete;
        Set<Oid> targetStorageComplete;

        auto iter = _index._targetCatalogIncomplete.find(oid);
        if(_index._targetCatalogIncomplete.end() != iter)
        {
            _index._targetCatalogIncomplete.erase(iter);

            if(match(o.get(), _targetCriterias))
            {
                collectObjectsIndex(
                            _targetCriterias,
                            oid,
                            o.get(),
                            targetCatalogIncomplete,
                            targetCatalogComplete,
                            targetStorageIncomplete,
                            targetStorageComplete,
                            &_index._targetCatalogComplete);
            }
            else
            {
                targetCatalogComplete.insert(oid);
            }
        }

        // buffer delta
        Set<Oid> bufferCatalogIncomplete;
        Set<Oid> bufferCatalogComplete;
        Set<Oid> bufferStorageIncomplete;
        Set<Oid> bufferStorageComplete;

        iter = _index._bufferCatalogIncomplete.find(oid);
        if(_index._bufferCatalogIncomplete.end() != iter)
        {
            _index._bufferCatalogIncomplete.erase(iter);

            if(match(o.get(), _bufferCriterias))
            {
                collectObjectsIndex(
                            _bufferCriterias,
                            oid,
                            o.get(),
                            bufferCatalogIncomplete,
                            bufferCatalogComplete,
                            bufferStorageIncomplete,
                            bufferStorageComplete,
                            &_index._bufferCatalogComplete);
            }
            else
            {
                bufferCatalogComplete.insert(oid);
            }
        }

        // apply delta
        _index._targetCatalogIncomplete .insert(targetCatalogIncomplete .begin(), targetCatalogIncomplete   .end());
        _index._targetCatalogComplete   .insert(targetCatalogComplete   .begin(), targetCatalogComplete     .end());
        _index._targetStorageIncomplete .insert(targetStorageIncomplete .begin(), targetStorageIncomplete   .end());
        _index._targetStorageComplete   .insert(targetStorageComplete   .begin(), targetStorageComplete     .end());

        _index._bufferCatalogIncomplete .insert(bufferCatalogIncomplete .begin(), bufferCatalogIncomplete   .end());
        _index._bufferCatalogComplete   .insert(bufferCatalogComplete   .begin(), bufferCatalogComplete     .end());
        _index._bufferStorageIncomplete .insert(bufferStorageIncomplete .begin(), bufferStorageIncomplete   .end());
        _index._bufferStorageComplete   .insert(bufferStorageComplete   .begin(), bufferStorageComplete     .end());

        // notify target
        for(const Oid& oid : targetCatalogIncomplete) _onTargetCatalogIncomplete.in(oid);
        for(const Oid& oid : targetCatalogComplete  ) _onTargetCatalogComplete  .in(oid);
        for(const Oid& oid : targetStorageIncomplete) _onTargetStorageIncomplete.in(oid);
        for(const Oid& oid : targetStorageComplete  ) _onTargetStorageComplete  .in(oid);

        if(!targetCatalogComplete.empty())
        {
            if(_index._targetCatalogIncomplete.empty() &&
               _index._targetStorageIncomplete.empty())
            {
                if(verbose)
                {
                    LOGI("target totally complete");
                }

                saveCatalog(false);
                _onTargetTotallyComplete.in();
            }
        }

        // notify buffer
        for(const Oid& oid : bufferCatalogIncomplete) _onBufferCatalogIncomplete.in(oid);
        for(const Oid& oid : bufferCatalogComplete  ) _onBufferCatalogComplete  .in(oid);
        for(const Oid& oid : bufferStorageIncomplete) _onBufferStorageIncomplete.in(oid);
        for(const Oid& oid : bufferStorageComplete  ) _onBufferStorageComplete  .in(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::updateIndexAfterStorageObjectComplete(bool verbose, const Oid& oid)
    {
        bool changed4Target = false;
        bool changed4Buffer = false;

        auto iter = _index._targetStorageIncomplete.find(oid);
        if(_index._targetStorageIncomplete.end() != iter)
        {
            changed4Target = true;
            _index._targetStorageIncomplete.erase(iter);
            _index._targetStorageComplete.insert(oid);
        }

        iter = _index._bufferStorageIncomplete.find(oid);
        if(_index._bufferStorageIncomplete.end() != iter)
        {
            changed4Buffer = true;
            _index._bufferStorageIncomplete.erase(iter);
            _index._bufferStorageComplete.insert(oid);
        }

        if(changed4Target)
        {
            _onTargetStorageComplete.in(oid);

            if(_index._targetCatalogIncomplete.empty() &&
               _index._targetStorageIncomplete.empty())
            {
                if(verbose)
                {
                    LOGI("target totally complete");
                }

                saveCatalog(false);
                _onTargetTotallyComplete.in();
            }
        }

        if(changed4Buffer)
        {
            _onBufferStorageComplete.in(oid);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::Index Instance::buildIndex()
    {
        Index res;

        res._allReleases = _catalog.enumerate(catalog::Object::Type::release);

        for(auto&[oid, r] : collectMostReleases(_targetCriterias, res._allReleases))
        {
            res._targetMostReleases.insert(oid);
            collectObjectsIndex(
                        _targetCriterias,
                        oid,
                        r.get(),
                        res._targetCatalogIncomplete,
                        res._targetCatalogComplete,
                        res._targetStorageIncomplete,
                        res._targetStorageComplete);
        }

        for(auto&[oid, r] : collectMostReleases(_bufferCriterias, res._allReleases))
        {
            res._bufferMostReleases.insert(oid);
            collectObjectsIndex(
                        _bufferCriterias,
                        oid,
                        r.get(),
                        res._bufferCatalogIncomplete,
                        res._bufferCatalogComplete,
                        res._bufferStorageIncomplete,
                        res._bufferStorageComplete);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<std::pair<Oid, catalog::ReleasePtr>> Instance::collectMostReleases(
            const std::vector<instance::Criteria>& criterias,
            const Set<Oid>& allReleases)
    {
        using Key = std::tuple<
            String,           // srcBranch
            String,           // platformOs
            String,           // platformArch
            //String,           // compiler
            //String,           // compilerVersion
            //String,           // compilerOptimization
            //uint32,           // stability
            String,           // vendor
            Array<uint8, 32>  // vendorSign
        >;
        struct Value
        {
            Oid                 _oid{};
            catalog::ReleasePtr _r;
            uint64              _srcMoment{};
        };

        std::map<Key, Value> mostReleases;

        for(const Oid& oid : allReleases)
        {
            catalog::ReleasePtr r = catalog::objectPtrCast<catalog::Release>(_catalog.get(oid));

            if(!r)
            {
                continue;
            }

            if(!match(r.get(), criterias))
            {
               continue;
            }

            //maximize moment
            Key key{
                r->_srcBranch,
                r->_platformOs,
                r->_platformArch,
                //r->_compiler,
                //r->_compilerVersion,
                //r->_compilerOptimization,
                //r->_stability,
                r->_vendor,
                r->_vendorSign};
            Value& prev = mostReleases[key];
            if(!prev._r || prev._srcMoment < r->_srcMoment)
            {
                prev._oid = oid;
                prev._srcMoment = r->_srcMoment;
                prev._r = std::move(r);
            }
        }

        Set<std::pair<Oid, catalog::ReleasePtr>> res;
        for(auto&[key, value] : mostReleases)
        {
            res.insert(std::make_pair(value._oid, std::move(value._r)));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::collectObjectsIndex(const std::vector<instance::Criteria>& criterias,
            const Oid& oid,
            catalog::Object* o,
            Set<Oid>& catalogIncomplete,
            Set<Oid>& catalogComplete,
            Set<Oid>& storageIncomplete,
            Set<Oid>& storageComplete,
            const Set<Oid>* globalCatalogComplete)
    {
        if(globalCatalogComplete && globalCatalogComplete->count(oid))
        {
            //already
            return;
        }

        if(catalogComplete.insert(oid).second)
        {
            if(catalog::Object::Type::file == o->type())
            {
                catalog::File* f = catalog::objectPtrCast<catalog::File>(o);
                if(_storage.has(f->_content))
                {
                    storageComplete.insert(f->_content);
                }
                else
                {
                    storageIncomplete.insert(f->_content);
                }
            }

            for(const Oid& depOid : o->_dependencies)
            {
                catalog::ObjectPtr depObject = _catalog.get(depOid);
                if(!depObject)
                {
                    catalogIncomplete.insert(depOid);
                    continue;
                }

                if(!match(depObject.get(), criterias))
                {
                    catalogComplete.insert(depOid);
                    continue;
                }

                collectObjectsIndex(
                            criterias,
                            depOid,
                            depObject.get(),
                            catalogIncomplete,
                            catalogComplete,
                            storageIncomplete,
                            storageComplete,
                            globalCatalogComplete);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Instance::match(const auto* catalogObject, const std::vector<instance::Criteria>& criterias)
    {
        for(const instance::Criteria& c : criterias)
        {
            if(c.match(catalogObject)) return true;
        }
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Instance::match(const auto* catalogObject, bool onlyTargetCriteria)
    {
        if(match(catalogObject, _targetCriterias) || (!onlyTargetCriteria && match(catalogObject, _bufferCriterias)))
        {
            return true;
        }
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::Index::reset()
    {
        _allReleases.clear();

        _targetMostReleases.clear();
        _targetCatalogIncomplete.clear();
        _targetCatalogComplete.clear();
        _targetStorageIncomplete.clear();
        _targetStorageComplete.clear();

        _bufferMostReleases.clear();
        _bufferCatalogIncomplete.clear();
        _bufferCatalogComplete.clear();
        _bufferStorageIncomplete.clear();
        _bufferStorageComplete.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::Index::swap(Index& other)
    {
        _allReleases.swap(other._allReleases);

        _targetMostReleases.swap(other._targetMostReleases);
        _targetCatalogIncomplete.swap(other._targetCatalogIncomplete);
        _targetCatalogComplete.swap(other._targetCatalogComplete);
        _targetStorageIncomplete.swap(other._targetStorageIncomplete);
        _targetStorageComplete.swap(other._targetStorageComplete);

        _bufferMostReleases.swap(other._bufferMostReleases);
        _bufferCatalogIncomplete.swap(other._bufferCatalogIncomplete);
        _bufferCatalogComplete.swap(other._bufferCatalogComplete);
        _bufferStorageIncomplete.swap(other._bufferStorageIncomplete);
        _bufferStorageComplete.swap(other._bufferStorageComplete);
    }
}
