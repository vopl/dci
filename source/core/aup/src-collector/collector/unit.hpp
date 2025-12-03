// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "meta.hpp"
#include "absAndRel.hpp"

namespace dci::aup::collector
{
    struct Target;
    struct Unit : Meta
    {
    public:
        using Meta::Meta;

        bool setup(const std::string& key, const std::vector<std::string>& values) override;
        Target* getTarget(const std::string& name, bool addIfMissing);

    public:
        std::set<AbsAndRel>             _srcDirs;
        std::set<AbsAndRel>             _srcFiles;

        std::set<AbsAndRel>             _includeDirs;
        std::set<AbsAndRel>             _includeFiles;

        std::set<AbsAndRel>             _idlDirs;
        std::set<AbsAndRel>             _idlFiles;

        std::set<AbsAndRel>             _cmmDirs;
        std::set<AbsAndRel>             _cmmFiles;

        std::set<std::string>           _extraAllowed;
        std::map<std::string, Target>   _targets;
    };
}
