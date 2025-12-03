// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <string>

namespace dci::utils::dns
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    enum class CanonicalizePartResult
    {
        unneeded,
        badOutput,
        ok,
        badInput,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS CanonicalizePartResult canonicalizePart(std::string_view src, std::string* dstPtr = nullptr);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    enum class CanonicalizeResult
    {
        unneeded,
        ip,
        ok,
        badInput,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    API_DCI_UTILS CanonicalizeResult canonicalize(std::string& domain, bool dotsOnSides = false);
}
