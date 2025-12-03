// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "keyIA.hpp"
#include "statIA.hpp"

namespace dci::module::ppn::connectivity::reest
{
    using StatsIA = std::map<KeyIA, StatIA>;
    using StatIARecord = std::pair<const KeyIA, StatIA>;

    const KeyIA& stat2Key(const StatIA& stat);
}
