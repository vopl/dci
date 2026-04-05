/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "applier.hpp"
#include "catalog.hpp"
#include "storage.hpp"
#include <dci/aup/catalog/identify.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <dci/utils/b2h.hpp>
#include <dci/utils/fnmatch.hpp>
#include <dci/crypto/rnd.hpp>
#include <dci/logger.hpp>
#include <fstream>

#define VERBOSE(msg) LOGI("applier: "<<msg)

namespace dci::aup::impl
{
    using namespace aup::catalog;
    using namespace aup::applier;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Applier::Applier()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Applier::~Applier()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::addCatalog(Catalog* c)
    {
        _catalogs.insert(c);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::addStorage(Storage* s)
    {
        _storages.insert(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::addRoot(const Oid& oid, const Set<aup::catalog::File::Kind>& fileKinds)
    {
        auto& fks = _roots[oid];
        for(aup::catalog::File::Kind fk : fileKinds)
        {
            fks.insert(fk);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    applier::Result Applier::process(const std::string &place, applier::Task task)
    {
        utils::AtScopeExit fin = {[&]
        {
            reset();
            _place.clear();
            _task = 0;
            _points.clear();
            _extraAllowed.clear();
        }};

        reset();
        _place = fs::weakly_canonical(place);
        _task = task;

        for(const auto&[oid, fks] : _roots)
        {
            uint64 res = traverse(oid, fks);
            if(res)
            {
                return static_cast<applier::Result>(res);
            }
        }

        traverse(_place);

        return static_cast<Result>(synchronize());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::reset()
    {
        _place.clear();
        _task = 0;
        _traversed.clear();
        _points.clear();
        _extraAllowed.clear();
        _emptyDirCandidates.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint64 Applier::traverse(const Oid& oid, const Set<File::Kind>& fileKinds)
    {
        if(!_traversed.insert(oid).second)
        {
            //already
            return rOk;
        }

        ObjectPtr o = fetchObject(oid);
        if(!o)
        {
            LOGE("catalog object incomplete: "<<utils::b2h(oid));
            return rIncompleteCatalog;
        }

        Set<Oid> dependencies;
        switch(o->type())
        {
        case Object::Type::release:
            dependencies = o->_dependencies;
            break;

        case Object::Type::unit:
            {
                UnitPtr u = objectPtrCast<Unit>(std::move(o));
                _extraAllowed.insert(u->_extraAllowed.begin(), u->_extraAllowed.end());
                dependencies = u->_dependencies;
            }
            break;

        case Object::Type::file:
            {
                FilePtr f = objectPtrCast<File>(std::move(o));

                if(!fileKinds.count(f->_kind))
                {
                    break;
                }

                dependencies = f->_dependencies;

                {
                    fs::path dir = _place/f->_path;
                    while(dir != _place)
                    {
                        dir = dir.parent_path();
                        _points[dir]._requiredAsDirectory = true;
                    }
                }

                Point& point = _points[_place/f->_path];

                if(!point._ideal)
                {
                    point._ideal = std::move(f);
                    point._idealOid = oid;
                }
                else
                {
                    if(point._ideal->_size != f->_size)
                    {
                        LOGE("file ambiguous by size: "<<point._ideal->_path);
                        return applier::rAmbiguousCatalog;
                    }

                    if(point._ideal->_content != f->_content)
                    {
                        LOGE("file ambiguous by content: "<<point._ideal->_path);
                        return applier::rAmbiguousCatalog;
                    }

                    if(point._ideal->_perms != f->_perms)
                    {
                        LOGE("file ambiguous by permissions: "<<point._ideal->_path);
                        return applier::rAmbiguousCatalog;
                    }

                    if(point._ideal->_dependencies != f->_dependencies)
                    {
                        LOGE("file ambiguous by dependencies: "<<point._ideal->_path);
                        return applier::rAmbiguousCatalog;
                    }
                }
            }
            break;

        default:
            LOGE("catalog corrupted");
            return rCorruptedCatalog;
        }

        for(const Oid& dep : dependencies)
        {
            uint64 res = traverse(dep, fileKinds);
            if(res)
            {
                return static_cast<applier::Result>(res);
            }
        }

        return rOk;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::traverse(const fs::path& dir)
    {
        if(!fs::exists(dir))
        {
            return;
        }

        for(auto entry : fs::recursive_directory_iterator(dir))
        {
            Point& point = _points[entry.path()];

            if(entry.is_regular_file())
            {
                if(point._requiredAsDirectory)
                {
                    point._realWrong = true;
                }
                else
                {
                    point._realFile = true;
                    point._realPerms = entry.status().permissions();
                    point._realSize = entry.file_size();
                    //point._realContent;
                }

                _emptyDirCandidates.insert(entry.path().parent_path());
            }
            else if(entry.is_directory())
            {
                if(point._ideal)
                {
                    point._realWrong = true;
                }
                _emptyDirCandidates.insert(entry.path());
            }
            else
            {
                point._realWrong = true;
                _emptyDirCandidates.insert(entry.path().parent_path());
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ObjectPtr Applier::fetchObject(const Oid& oid)
    {
        ObjectPtr res;

        for(Catalog* c : _catalogs)
        {
            res = c->get(oid);
            if(res)
            {
                return res;
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Applier::hasContent(const Oid& oid)
    {
        for(Storage* s : _storages)
        {
            if(s->has(oid))
            {
                return true;
            }
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Applier::fetchContent(const Oid& oid)
    {
        std::optional<Bytes> res;

        for(Storage* s : _storages)
        {
            res = s->get(oid);
            if(res)
            {
                return res;
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Applier::deleteContent(const Oid& oid)
    {
        for(Storage* s : _storages)
        {
            s->del(oid);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Applier::evaluateContentCheck(const fs::path& p)
    {
        std::ifstream in{p.native().c_str(), std::ios_base::in | std::ios_base::binary};

        if(!in)
        {
            throw std::system_error(errno, std::generic_category(), "unable to open "+p.string());
        }

        dci::Bytes blob;
        dci::bytes::Alter a {blob.end()};
        for(;;)
        {
            uint32 bufSize;
            void* buf = a.prepareWriteBuffer(bufSize);
            if(!in.read(static_cast<char *>(buf), bufSize))
            {
                if(in.eof())
                {
                    bufSize = static_cast<uint32>(in.gcount());
                    a.commitWriteBuffer(bufSize);
                    break;
                }
                else
                {
                    throw std::system_error(errno, std::generic_category(), "unable to read "+p.string());
                }
            }

            bufSize = static_cast<uint32>(in.gcount());
            a.commitWriteBuffer(bufSize);
        }

        return dci::aup::catalog::identify(blob);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint64 Applier::synchronize()
    {
        uint64 res = 0;

        for(auto &[path, point] : _points)
        {
            if(point._ideal)
            {
                if(_task & tCheckStorage)
                {
                    auto blob = fetchContent(point._ideal->_content);
                    if(!blob)
                    {
                        if(_task & tVerboseMajor) VERBOSE("storage missing "<<path.lexically_proximate(_place));
                        res |= rIncompleteStorage;
                    }
                    else if(point._ideal->_content != dci::aup::catalog::identify(*blob))
                    {
                        if(_task & tVerboseMajor) VERBOSE("storage corrupted "<<path.lexically_proximate(_place));
                        deleteContent(point._ideal->_content);
                        res |= rIncompleteStorage;
                    }
                }
                else
                {
                    if(!hasContent(point._ideal->_content))
                    {
                        if(_task & tVerboseMajor) VERBOSE("storage missing "<<path.lexically_proximate(_place));
                        res |= rIncompleteStorage;
                    }
                }
            }
        }

        if(res)
        {
            return res;
        }

        for(auto&[path, point] : _points)
        {
            if(point._realWrong || (point._requiredAsDirectory && point._ideal))
            {
                if(_task & tRemoveWrongs)
                {
                    if(_task & tVerboseMajor) VERBOSE("remove wrong "<<path.lexically_proximate(_place));
                    res |= remove(path);
                }
                else
                {
                    if(_task & tVerboseMinor) VERBOSE("wrong "<<path.lexically_proximate(_place));
                }
            }
        }

        //remove unwanted files
        for(auto&[path, point] : _points)
        {
            if(point._realFile && !point._ideal)
            {
                if(extraAllowed(path))
                {
                    //if(_task & tVerboseMinor) VERBOSE("extra allowed "<<path.lexically_proximate(_place));
                }
                else
                {
                    if(_task & tRemoveExtra)
                    {
                        if(_task & tVerboseMajor) VERBOSE("remove extra "<<path.lexically_proximate(_place));
                        res |= remove(path);
                    }
                    else
                    {
                        if(_task & tVerboseMinor) VERBOSE("extra "<<path.lexically_proximate(_place));
                        res |= rExistsExtra;
                    }
                }
            }
        }

        //create, update, fix permissions for files
        for(auto&[path, point] : _points)
        {
            _emptyDirCandidates.insert(path.parent_path());

            if(!point._realFile && point._ideal)
            {
                if(_task & tEmplaceMissings)
                {
                    if(_task & tVerboseMajor) VERBOSE("emplace missing "<<path.lexically_proximate(_place));
                    res |= emplace(path, permsFor(point._ideal.get()), point._ideal->_content);
                }
                else
                {
                    if(_task & tVerboseMinor) VERBOSE("missing "<<path.lexically_proximate(_place));
                    res |= rExistsMissings;
                }
            }
            else if(point._realFile && point._ideal)
            {
                if(point._realSize != point._ideal->_size)
                {
                    if(_task & tEmplaceChanges)
                    {
                        if(_task & tVerboseMajor) VERBOSE("wrong size, update "<<path.lexically_proximate(_place));
                        res |= update(path, permsFor(point._ideal.get()), point._ideal->_content);
                    }
                    else
                    {
                        if(_task & tVerboseMinor) VERBOSE("wrong size "<<path.lexically_proximate(_place));
                        res |= rExistsChanges;
                    }
                }
                else if(evaluateContentCheck(path) != point._ideal->_content)
                {
                    if(_task & tEmplaceChanges)
                    {
                        if(_task & tVerboseMajor) VERBOSE("wrong content, update "<<path.lexically_proximate(_place));
                        res |= update(path, permsFor(point._ideal.get()), point._ideal->_content);
                    }
                    else
                    {
                        if(_task & tVerboseMinor) VERBOSE("wrong content "<<path.lexically_proximate(_place));
                        res |= rExistsChanges;
                    }
                }
                else if(point._realPerms != permsFor(point._ideal.get()))
                {
                    if(_task & tEmplaceChanges)
                    {
                        if(_task & tVerboseMajor) VERBOSE("wrong permissions, fix "<<path.lexically_proximate(_place));
                        fs::permissions(path, permsFor(point._ideal.get()));
                    }
                    else
                    {
                        if(_task & tVerboseMinor) VERBOSE("wrong permissions "<<path.lexically_proximate(_place));
                        res |= rExistsChanges;
                    }
                }
                else
                {
                    //all equal
                }
            }
        }

        //remove empty directories
        while(!_emptyDirCandidates.empty())
        {
            std::set<fs::path> tmp;
            tmp.swap(_emptyDirCandidates);

            for(const fs::path& path : tmp)
            {
                fs::path rp = path.lexically_relative(_place);
                if(rp.empty() || ".." == *rp.begin())
                {
                    continue;
                }

                if(fs::is_directory(path) && fs::is_empty(path))
                {
                    if(extraAllowed(path))
                    {
                        //if(_task & tVerboseMinor) VERBOSE("empty dir allowed "<<path.lexically_proximate(_place));
                    }
                    else
                    {
                        if(_task & tRemoveExtra)
                        {
                            if(_task & tVerboseMajor) VERBOSE("remove empty dir "<<path.lexically_proximate(_place));
                            res |= remove(path);
                            _emptyDirCandidates.insert(path.parent_path());
                        }
                        else
                        {
                            if(_task & tVerboseMinor) VERBOSE("empty dir "<<path.lexically_proximate(_place));
                        }
                    }
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint64 Applier::emplace(const fs::path& path, std::filesystem::perms perms, const Oid& content)
    {
        auto blob = fetchContent(content);
        if(!blob)
        {
            return rIncompleteStorage;
        }

        fs::create_directories(path.parent_path());

        {
            std::ofstream out{path.native().c_str(), std::ios_base::out | std::ios_base::binary};
            if(!out)
            {
                throw std::system_error(errno, std::generic_category(), "unable to open "+path.string());
            }
            bytes::Cursor c {blob->begin()};
            while(!c.atEnd())
            {
                if(!out.write(static_cast<const char *>(static_cast<const void *>(c.continuousData())), c.continuousDataSize()))
                {
                    throw std::system_error(errno, std::generic_category(), "unable to write "+path.string());
                }

                c.advanceChunks(1);
            }
        }

        fs::permissions(path, perms);

        return rFixedMissings;
    }

    namespace
    {
        std::string rndExtension()
        {
            char buf[32];
            dci::crypto::rnd::generate(buf, sizeof(buf));
            return dci::utils::b2h(buf, sizeof(buf));
        }
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint64 Applier::remove(const fs::path& path)
    {
        if(fs::exists(path))
        {
#ifdef _WIN32
            std::error_code ec;
            fs::remove_all(path, ec);
            if(ec)
            {
                if(_task & tVerboseMinor)
                {
                    VERBOSE("unable to remove "<<path<<": "<<ec);
                }

                fs::path tmp = path;
                tmp.replace_extension(rndExtension());
                while(fs::exists(tmp))
                {
                    tmp.replace_extension(rndExtension());
                }

                fs::rename(path, tmp, ec);
                if(ec)
                {
                    if(_task & tVerboseMinor)
                    {
                        VERBOSE("unable to rename "<<path<<" -> "<<tmp<<": "<<ec);
                    }

                    return rExistsExtra;
                }
                else
                {
                    if(_task & tVerboseMinor)
                    {
                        VERBOSE("renamed "<<path<<" -> "<<tmp);
                    }
                }
            }
#else
            fs::remove_all(path);
#endif
            return rFixedExtra;
        }

        return rOk;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint64 Applier::update(const fs::path& path, std::filesystem::perms perms, const Oid& content)
    {
        fs::path tmp = path;
        tmp.replace_extension(rndExtension());
        while(fs::exists(tmp))
        {
            tmp.replace_extension(rndExtension());
        }
        emplace(tmp, perms, content);
        if(!(rSomeWrong & remove(path)))
        {
            fs::rename(tmp, path);
            return rFixedChanges;
        }

        return rExistsChanges;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    fs::perms Applier::permsFor(File* f)
    {
        return static_cast<fs::perms>(f->_perms);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Applier::extraAllowed(std::filesystem::path path)
    {
        if(path.is_absolute())
        {
            path = path.lexically_relative(_place);
        }

        for(const std::string& pattern : _extraAllowed)
        {
            if(dci::utils::fnmatch(pattern.c_str(), path.string().c_str(), dci::utils::fnmPathName | dci::utils::fnmNoEscape))
            {
                return true;
            }
        }

        return false;
    }
}
