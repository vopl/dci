// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "keyI.hpp"
#include "statI.hpp"

namespace dci::module::ppn::connectivity::reest
{
    using StatsI = std::map<KeyI, StatI>;
    using StatIRecord = std::pair<const KeyI, StatI>;

    const KeyI& stat2Key(const StatI& stat);
}
