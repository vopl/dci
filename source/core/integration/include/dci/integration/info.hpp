// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <string_view>
#include <string>
#include <cstdint>

namespace dci::integration::info
{
    std::string_view API_DCI_INTEGRATION srcBranch();
    std::string_view API_DCI_INTEGRATION srcRevision();
    std::uint64_t    API_DCI_INTEGRATION srcMoment();
    std::string_view API_DCI_INTEGRATION platformOs();
    std::string_view API_DCI_INTEGRATION platformArch();
    std::string_view API_DCI_INTEGRATION compiler();
    std::string_view API_DCI_INTEGRATION compilerVersion();
    std::string_view API_DCI_INTEGRATION compilerOptimization();
    std::string_view API_DCI_INTEGRATION vendor();

    std::string_view API_DCI_INTEGRATION version();
    std::string      API_DCI_INTEGRATION version(std::string_view srcBranch, std::string_view srcRevision, std::uint64_t srcMoment);
}
