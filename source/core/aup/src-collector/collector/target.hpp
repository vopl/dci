// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/aup/catalog/file.hpp>
#include "meta.hpp"
#include "absAndRel.hpp"

namespace dci::aup::collector
{
    struct Target : Meta
    {
    public:
        using Meta::Meta;

        bool setup(const std::string& key, const std::vector<std::string>& values) override;

    public:
        catalog::File::Kind   _kind{};
        fs::path              _file;
        std::set<fs::path>    _deps;

    public:
        std::set<AbsAndRel>   _resourceDeps;
    };
}
