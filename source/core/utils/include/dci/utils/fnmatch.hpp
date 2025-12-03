// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"

namespace dci::utils
{
    enum FnmatchFlags : int
    {
        fnmNoEscape     =0x01,
        fnmPathName     =0x02,
        fnmPeriod       =0x04,
        fnmLeadingDir   =0x08,
        fnmCaseFold     =0x10,

        fnmIgnoreCase   =fnmCaseFold,
        fnmFileName     =fnmPathName,
    };

    API_DCI_UTILS bool fnmatch(const char* pattern, const char* string, int flags = 0);
}
