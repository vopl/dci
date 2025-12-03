// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <chrono>
#include <string>

namespace dci::logger
{
    using TimeProvider = std::chrono::system_clock::time_point(*)();

    TimeProvider API_DCI_LOGGER setTimeProvider(TimeProvider);
    std::string_view API_DCI_LOGGER timeProvidedAsString();
}
