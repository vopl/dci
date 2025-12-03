// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "../collector.hpp"
#include <dci/crypto/ed25519.hpp>
#include <dci/integration/info.hpp>
#include <dci/utils/fnmatch.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <fstream>
#include <cassert>
#include <regex>

namespace dci::aup
{
    namespace
    {
        void operator +=(std::set<Oid>& dst, const Oid& src)
        {
            dst.insert(src);
        }

        void operator +=(std::set<Oid>& dst, const std::set<Oid>& src)
        {
            dst.insert(src.begin(), src.end());
        }

        void operator +=(std::set<fs::path>& dst, const fs::path& src)
        {
            dst.insert(src);
        }

//        void operator +=(std::set<fs::path>& dst, const std::set<fs::path>& src)
//        {
//            dst.insert(src.begin(), src.end());
//        }

        void operator +=(std::set<collector::AbsAndRel>& dst, const collector::AbsAndRel& src)
        {
            dst.insert(src);
        }

        void operator +=(std::set<collector::AbsAndRel>& dst, const std::set<collector::AbsAndRel>& src)
        {
            dst.insert(src.begin(), src.end());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Collector::processFileContent(const fs::path& p)
    {
        assert(p.is_absolute());

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

        Oid oid = catalog::identify(blob);
        _aupStorage.put(oid, std::move(blob));

        return oid;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    collector::AbsAndRel Collector::absAndRel(const collector::Meta& meta, const fs::path& file)
    {
        assert(file.is_absolute());
        assert(file == file.lexically_normal());

        for(const auto&[pattern, dir] : meta._syslibMapTo)
        {
            if(dci::utils::fnmatch(pattern.c_str(), file.string().c_str(), dci::utils::fnmFileName | dci::utils::fnmNoEscape))
            {
                collector::AbsAndRel res;
                res.abs() = file;
                res.rel() = dir / file.filename();
                return res;
            }
        }

        collector::AbsAndRel longest;
        std::set<collector::AbsAndRel> mappings = meta._dirMapping;
        mappings += meta._resourceDirs;
        for(const collector::AbsAndRel& mapping : mappings)
        {
            fs::path rel = file.lexically_proximate(mapping.abs());
            if(!rel.empty() && ".." != *rel.begin())
            {
                if(longest.abs().string().size() < mapping.abs().string().size())
                {
                    longest.abs() = file;
                    longest.rel() = mapping.rel() / rel;
                }
            }
        }

        if(!longest.abs().empty())
        {
            return longest;
        }

        for(const std::string& pattern : meta._syslibIgnore)
        {
            if(dci::utils::fnmatch(pattern.c_str(), file.string().c_str(), dci::utils::fnmFileName | dci::utils::fnmNoEscape))
            {
                return {};
            }
        }

        if(meta._parent)
        {
            return absAndRel(*meta._parent, file);
        }

        throw std::runtime_error{"unable to map path: " + file.string()};
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    collector::AbsAndRel Collector::absAndRel(const collector::Meta& meta, const collector::AbsAndRel& file)
    {
        (void)meta;
        return file;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<collector::AbsAndRel> Collector::processFileDebug(const collector::Meta& meta, const collector::AbsAndRel& file)
    {
        (void)meta;

        std::string dbgFile;
#ifdef _WIN32
        fs::path dbgFileAbs = file.abs();
        dbgFileAbs.replace_extension(dbgFileAbs.extension().string()+".debug");
        if(!fs::exists(dbgFileAbs))
        {
            return{};
        }
        dbgFile = dbgFileAbs.filename().string();
#else
        {
            std::string cmd = std::string{"readelf --string-dump=.gnu_debuglink "} + file.abs().string() + " 2>&1";
            FILE* pipe = popen(cmd.c_str(), "re");
            utils::AtScopeExit se{[&]
            {
                if(pipe)
                {
                    pclose(pipe);
                    pipe = nullptr;
                }
            }};

            if(!pipe)
            {
                std::cerr << "unable to execute: " << cmd << ", " << std::error_code{errno, std::system_category()}.message() << std::endl;
                return {};
            }

            std::string out;
            while(!feof(pipe))
            {
                char buffer[128];
                if(std::size_t readed = fread(buffer, 1, 128, pipe))
                {
                    out.append(buffer, readed);
                }
            }

            int res = pclose(pipe);
            pipe = nullptr;

            if(0 == res)
            {
                static const std::regex rex1{R"(\n\s*\[\s*0+\s*\]\s*(.+)\s*\n)"};

                std::smatch match;
                if(std::regex_search(out, match, rex1))
                {
                    dbgFile = match[1];
                }
            }
        }

        if(dbgFile.empty())
        {
            return {};
        }
#endif

        collector::AbsAndRel dbg = file;
        dbg.abs().replace_filename(dbgFile);
        dbg.rel().replace_filename(dbgFile);

        //std::cout << dbgAbs.string() << std::endl;
        if(!fs::exists(dbg.abs()))
        {
            std::cerr << "no " << dbgFile << " found near " << file.abs() << std::endl;
            return {};
        }

        if(!_processedFiles.contains(dbg))
        {
            catalog::FilePtr dbgF{std::make_unique<catalog::File>()};
            dbgF->_kind = catalog::File::Kind::debug;
            dbgF->_path = dbg.rel().string();
            dbgF->_perms = static_cast<uint16>(fs::status(dbg.abs()).permissions()) & 0777;
            dbgF->_size = static_cast<uint32>(fs::file_size(dbg.abs()));
            dbgF->_content = processFileContent(dbg.abs());

            _processedFiles[dbg] = _aupCatalog.put(std::move(dbgF));
        }

        return {dbg};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<collector::AbsAndRel>  Collector::processFile(const collector::Meta& meta, const collector::AbsAndRel& file, catalog::File::Kind kind, const std::set<collector::AbsAndRel>& deps)
    {
        assert(file.abs().is_absolute());
        assert(file.abs() == file.abs().lexically_normal());

        if(file.abs().empty())
        {
            return {};
        }

        if(_processedFiles.contains(file))
        {
            return {};
        }

        catalog::FilePtr f{std::make_unique<catalog::File>()};

        for(const collector::AbsAndRel& dep : deps)
        {
            auto iter = _processedFiles.find(dep);
            if(_processedFiles.end() == iter)
            {
                throw std::runtime_error{"unresolved dependency for target " + meta._name + ": " + dep.abs().string() + "[" + dep.rel().string() + "]"};
            }

            f->_dependencies += iter->second;
        }

        f->_kind = kind;
        f->_path = file.rel().string();
        f->_perms = static_cast<uint16>(fs::status(file.abs()).permissions()) & 0777;
        f->_size = static_cast<uint32>(fs::file_size(file.abs()));
        f->_content = processFileContent(file.abs());

        _processedFiles[file] = _aupCatalog.put(std::move(f));
        return {file};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<collector::AbsAndRel>  Collector::processFile(const collector::Meta& meta, const fs::path& file, catalog::File::Kind kind, const std::set<collector::AbsAndRel>& deps)
    {
        assert(file.is_absolute());
        assert(file == file.lexically_normal());

        return processFile(meta, absAndRel(meta, file), kind, deps);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<collector::AbsAndRel> Collector::processFiles(const collector::Meta& meta, const auto& files, catalog::File::Kind kind)
    {
        std::set<collector::AbsAndRel> res;

        for(const auto& file : files)
        {
            res += processFile(meta, file, kind, {});
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<collector::AbsAndRel>  Collector::processDirs(const collector::Meta& meta, const std::set<collector::AbsAndRel>& dirs, catalog::File::Kind kind)
    {
        std::set<collector::AbsAndRel>  res;

        for(const collector::AbsAndRel& dir : dirs)
        {
            for(auto entry : fs::recursive_directory_iterator(dir.abs()))
            {
                if(entry.is_regular_file())
                {
                    fs::path absFile = (dir.abs()/entry.path()).lexically_normal();
                    fs::path relFile = (dir.rel()/entry.path().lexically_relative(dir.abs())).lexically_normal();
                    res += processFile(meta, collector::AbsAndRel{absFile, relFile}, kind, {});
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::processFiles()
    {
        // вырезать системный игнор
        for(auto& nu: _unitsMeta)
        {
            collector::Unit& u = nu.second;

            for(auto& nt: u._targets)
            {
                collector::Target& t = nt.second;

                for(auto iter = t._deps.begin(); iter != t._deps.end();)
                {
                    if(absAndRel(t, *iter).abs().empty())
                    {
                        iter = t._deps.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }
                }
            }
        }

        // сначала то что ни от чего не зависит
        for(auto& nu: _unitsMeta)
        {
            collector::Unit& u = nu.second;

            for(auto& nt: u._targets)
            {
                collector::Target& t = nt.second;
                if(catalog::File::Kind::null == t._kind)
                {
                    continue;
                }

                t._resourceDeps += processFiles(t, t._resourceFiles, catalog::File::Kind::resource);
                t._resourceDeps += processDirs(t, t._resourceDirs, catalog::File::Kind::resource);
            }

            processFiles(u, u._resourceFiles, catalog::File::Kind::resource);
            processDirs(u, u._resourceDirs, catalog::File::Kind::resource);

            if(!_ignoreSources)
            {
                processDirs(u, u._srcDirs, catalog::File::Kind::src);
                processFiles(u, u._srcFiles, catalog::File::Kind::src);
            }

            processDirs(u, u._includeDirs, catalog::File::Kind::include);
            processFiles(u, u._includeFiles, catalog::File::Kind::include);

            processDirs(u, u._idlDirs, catalog::File::Kind::idl);
            processFiles(u, u._idlFiles, catalog::File::Kind::idl);

            processDirs(u, u._cmmDirs, catalog::File::Kind::cmm);
            processFiles(u, u._cmmFiles, catalog::File::Kind::cmm);
        }

        // собрать все таргеты
        std::set<fs::path> targetFiles;
        for(const auto& nu: _unitsMeta)
        {
            const collector::Unit& u = nu.second;
            for(const auto& nt: u._targets)
            {
                const collector::Target& t = nt.second;
                if(catalog::File::Kind::null == t._kind)
                {
                    continue;
                }

                targetFiles += t._file;
            }
        }

        // теперь все зависимости таргетов кроме самих таргетов
        for(const auto& nu: _unitsMeta)
        {
            const collector::Unit& u = nu.second;
            for(const auto& nt: u._targets)
            {
                const collector::Target& t = nt.second;
                if(catalog::File::Kind::null == t._kind)
                {
                    continue;
                }

                for(const fs::path& dep : t._deps)
                {
                    if(!targetFiles.contains(dep))
                    {
                        std::set<collector::AbsAndRel> dbgDeps;
                        if(!_ignoreDebug4Others)
                        {
                            dbgDeps += processFileDebug(t, absAndRel(t, dep));
                        }

                        processFile(t, dep, catalog::File::Kind::rdep, dbgDeps);
                    }
                }
            }
        }

        // теперь таргеты, с сортировкой по зависимостям между собой
        {
            std::size_t unprocessed = 0;
            std::size_t processed = 0;
            std::set<collector::AbsAndRel> cumulativeUnresolvedDeps;

            do
            {
                unprocessed = 0;
                processed = 0;
                cumulativeUnresolvedDeps.clear();

                for(const auto& nu: _unitsMeta)
                {
                    const collector::Unit& u = nu.second;
                    for(const auto& nt: u._targets)
                    {
                        const collector::Target& t = nt.second;
                        if(catalog::File::Kind::null == t._kind)
                        {
                            continue;
                        }

                        std::set<collector::AbsAndRel> allDeps;
                        for(const fs::path& p : t._deps)
                        {
                            allDeps += absAndRel(t, p);
                        }
                        allDeps += t._resourceDeps;

                        std::set<collector::AbsAndRel> unresolvedDeps;
                        for(const collector::AbsAndRel& dep : allDeps)
                        {
                            if(!_processedFiles.contains(dep))
                            {
                                unresolvedDeps.insert(dep);
                            }
                        }

                        if(unresolvedDeps.empty())
                        {
                            if(!_ignoreDebug4Targets)
                            {
                                allDeps += processFileDebug(t, absAndRel(t, t._file));
                            }

                            if(!processFile(t, t._file, t._kind, allDeps).empty())
                            {
                                processed++;
                            }
                        }
                        else
                        {
                            cumulativeUnresolvedDeps += unresolvedDeps;
                            unprocessed++;
                        }
                    }
                }
            }
            while(processed);

            if(unprocessed)
            {
                std::string cumulativeUnresolvedDepsString;
                for(const collector::AbsAndRel& udep : cumulativeUnresolvedDeps)
                {
                    if(!cumulativeUnresolvedDepsString.empty())
                    {
                        cumulativeUnresolvedDepsString += "; ";
                    }

                    cumulativeUnresolvedDepsString += udep.abs().string() + "[" + udep.rel().string() + "]";
                }
                throw std::runtime_error{"unable to order targets by its dependencies, some unresolveds: " + cumulativeUnresolvedDepsString};
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Collector::fixFile(const collector::AbsAndRel& file)
    {
        auto iter = _processedFiles.find(file);
        if(_processedFiles.end() == iter)
        {
            throw std::runtime_error{"file isnt processed for fixation: " + file.abs().string() + "[" + file.rel().string() + "]"};
        }

        return iter->second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<Oid> Collector::fixFiles(const std::set<collector::AbsAndRel>& files)
    {
        std::set<Oid> res;
        for(const collector::AbsAndRel& file : files)
        {
            res += fixFile(file);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<Oid> Collector::fixDirs(const std::set<collector::AbsAndRel>& dirs)
    {
        std::set<Oid> res;
        for(const collector::AbsAndRel& dir : dirs)
        {
            for(const fs::directory_entry &entry : fs::recursive_directory_iterator(dir.abs()))
            {
                if(entry.is_regular_file())
                {
                    fs::path relFile = entry.path().lexically_relative(dir.abs());
                    collector::AbsAndRel file{dir};
                    file.abs() /= relFile;
                    file.rel() /= relFile;
                    res += fixFile(file);
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::set<Oid> Collector::fixTarget(const collector::Target& meta)
    {
        if(catalog::File::Kind::null == meta._kind)
        {
            return {};
        }

        std::set<Oid> res;

        res += fixFile(absAndRel(meta, meta._file));

        std::set<collector::AbsAndRel> deps;
        for(const fs::path& dep : meta._deps)
        {
            deps += absAndRel(meta, dep);
        }
        res += fixFiles(deps);

        res += fixDirs (meta._resourceDirs);
        res += fixFiles(meta._resourceFiles);

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Collector::fixUnit(const collector::Unit& meta)
    {
        catalog::UnitPtr unit{std::make_unique<catalog::Unit>()};
        unit->_name = meta._name;
        unit->_extraAllowed = meta._extraAllowed;

        for(const auto& ntarget: meta._targets)
        {
            if(catalog::File::Kind::null == ntarget.second._kind)
            {
                continue;
            }

            unit->_dependencies += fixTarget(ntarget.second);
        }

        if(!_ignoreSources)
        {
            unit->_dependencies += fixDirs (meta._srcDirs);
            unit->_dependencies += fixFiles(meta._srcFiles);
        }

        unit->_dependencies += fixDirs (meta._includeDirs);
        unit->_dependencies += fixFiles(meta._includeFiles);
        unit->_dependencies += fixDirs (meta._idlDirs);
        unit->_dependencies += fixFiles(meta._idlFiles);
        unit->_dependencies += fixDirs (meta._cmmDirs);
        unit->_dependencies += fixFiles(meta._cmmFiles);
        unit->_dependencies += fixDirs (meta._resourceDirs);
        unit->_dependencies += fixFiles(meta._resourceFiles);

        return _aupCatalog.put(std::move(unit));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Collector::fixRelease()
    {
        catalog::ReleasePtr release{std::make_unique<catalog::Release>()};

        for(const auto& nmeta: _unitsMeta)
        {
            release->_dependencies += fixUnit(nmeta.second);
        }

        {
            release->_srcBranch = dci::integration::info::srcBranch();
            release->_srcRevision = dci::integration::info::srcRevision();
            release->_srcMoment = dci::integration::info::srcMoment();

            release->_platformOs = dci::integration::info::platformOs();
            release->_platformArch = dci::integration::info::platformArch();
            release->_compiler = dci::integration::info::compiler();
            release->_compilerVersion = dci::integration::info::compilerVersion();
            release->_compilerOptimization = dci::integration::info::compilerOptimization();

            release->_vendor = dci::integration::info::vendor();
            release->_stability = 0;
        }

        {
            dci::crypto::ed25519::mkPublic(_vendorKey.data(), release->_vendorSign.data());
            release->_signature.fill(0);
            Oid releaseHash = catalog::identify(release.get());
            dci::crypto::ed25519::sign(
                        releaseHash.data(), releaseHash.size(),
                        release->_vendorSign.data(), _vendorKey.data(),
                        release->_signature.data());
        }

        _aupCatalog.put(std::move(release));
    }
}
