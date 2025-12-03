// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <tuple>
#include <filesystem>

namespace dci::aup::collector
{
    namespace fs = std::filesystem;

    struct AbsAndRel : std::pair<fs::path, fs::path>
    {
        using pair::pair;

              fs::path& abs()       {return std::get<0>(*this);}
        const fs::path& abs() const {return std::get<0>(*this);}
              fs::path& rel()       {return std::get<1>(*this);}
        const fs::path& rel() const {return std::get<1>(*this);}

        auto operator<=>(const AbsAndRel&) const = default;
    };
}
