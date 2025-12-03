// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/aup/oid.hpp>
#include <dci/aup/applier/task.hpp>
#include <dci/aup/applier/result.hpp>
#include <dci/aup/catalog/object.hpp>
#include <dci/aup/catalog/release.hpp>
#include <dci/aup/catalog/unit.hpp>
#include <dci/aup/catalog/file.hpp>
#include <vector>
#include <set>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

namespace dci::aup::impl
{
    class Catalog;
    class Storage;

    class Applier final
    {
        Applier(const Applier&) = delete;
        Applier(Applier&&) = delete;

        void operator=(const Applier&) = delete;
        void operator=(Applier&&) = delete;

    public:
        Applier();
        ~Applier();

    public:
        void addCatalog(Catalog* c);
        void addStorage(Storage* s);
        void addRoot(const Oid& oid, const Set<aup::catalog::File::Kind>& fileKinds);

    public:
        applier::Result process(const std::string& place, applier::Task task = applier::tNull);

    private:
        void reset();
        uint64 traverse(const Oid& oid, const Set<aup::catalog::File::Kind>& fileKinds);
        void traverse(const fs::path& dir);
        aup::catalog::ObjectPtr fetchObject(const Oid& oid);

        bool hasContent(const Oid& oid);
        std::optional<Bytes> fetchContent(const Oid& oid);
        void deleteContent(const Oid& oid);
        Oid evaluateContentCheck(const fs::path& p);

    private:
        uint64 synchronize();
        uint64 emplace(const fs::path& path, fs::perms perms, const Oid& content);
        uint64 remove(const fs::path& path);
        uint64 update(const fs::path& path, fs::perms perms, const Oid& content);

        static fs::perms permsFor(aup::catalog::File* f);
        bool extraAllowed(fs::path path);

    private:
        std::set<Catalog *>                             _catalogs;
        std::set<Storage *>                             _storages;
        std::map<Oid, Set<aup::catalog::File::Kind>>    _roots;

    private:
        fs::path    _place;
        uint64      _task {};

    private:
        struct Point
        {
            bool                    _requiredAsDirectory {};

            aup::catalog::FilePtr   _ideal;
            Oid                     _idealOid {};

            bool                    _realWrong {};

            bool                    _realFile;
            fs::perms               _realPerms  {fs::perms::unknown};
            uint64                  _realSize   {};
        };

        std::set<Oid>               _traversed;
        std::map<fs::path, Point>   _points;
        std::set<std::string>       _extraAllowed;

        std::set<fs::path>          _emptyDirCandidates;
    };
}
