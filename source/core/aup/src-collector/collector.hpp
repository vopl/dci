// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <vector>
#include <set>
#include <string>
#include <filesystem>
#include <dci/aup.hpp>
#include "collector/unit.hpp"
#include "collector/target.hpp"

namespace dci::aup
{
    namespace fs = std::filesystem;

    class Collector
    {
    public:
        Collector();
        ~Collector();

        void setMetaFile(std::string v);

        void setVendorKey(std::string v);
        void setStorageDir(std::string v);

        void setIgnoreSources(bool v);
        void setIgnoreDebug4Targets(bool v);
        void setIgnoreDebug4Others(bool v);

        void run();

    private:
        void loadMeta();

    private://processing
        Oid processFileContent(const fs::path& p);

        collector::AbsAndRel absAndRel(const collector::Meta& meta, const fs::path& file);
        collector::AbsAndRel absAndRel(const collector::Meta& meta, const collector::AbsAndRel& file);

        std::set<collector::AbsAndRel> processFileDebug(const collector::Meta& meta, const collector::AbsAndRel& file);

        std::set<collector::AbsAndRel> processFile(const collector::Meta& meta, const collector::AbsAndRel& file, catalog::File::Kind kind, const std::set<collector::AbsAndRel>& deps);
        std::set<collector::AbsAndRel> processFile(const collector::Meta& meta, const fs::path& file, catalog::File::Kind kind, const std::set<collector::AbsAndRel>& deps);

        std::set<collector::AbsAndRel> processFiles(const collector::Meta& meta, const auto& files, catalog::File::Kind kind);
        std::set<collector::AbsAndRel> processDirs(const collector::Meta& meta, const std::set<collector::AbsAndRel>& dirs, catalog::File::Kind kind);
        void processFiles();

    private://fixation
        Oid fixFile(const collector::AbsAndRel& file);
        std::set<Oid> fixFiles(const std::set<collector::AbsAndRel>& files);
        std::set<Oid> fixDirs(const std::set<collector::AbsAndRel>& dirs);
        std::set<Oid> fixTarget(const collector::Target& meta);
        Oid fixUnit(const collector::Unit& meta);
        void fixRelease();

    private:
        fs::path                                _metaFile;
        std::array<std::uint8_t, 32>            _vendorKey {};
        fs::path                                _storageDir;
        bool                                    _ignoreSources {true};
        bool                                    _ignoreDebug4Targets {true};
        bool                                    _ignoreDebug4Others {true};

    private:
        collector::Meta                         _globalMeta;
        std::map<std::string, collector::Unit>  _unitsMeta;

    private:
        Catalog                                 _aupCatalog;
        Storage                                 _aupStorage;

    private:
        std::map<collector::AbsAndRel, Oid>     _processedFiles;
    };
}
