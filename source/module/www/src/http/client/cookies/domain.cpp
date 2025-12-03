// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "domain.hpp"

namespace dci::module::www::http::client::cookies::domain
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    dci::utils::dns::CanonicalizeResult canonicalize(std::string& domain)
    {
        return dci::utils::dns::canonicalize(domain, true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool matched(std::string_view target, std::string_view pattern)
    {
        dbgAssert(!target.empty());
        dbgAssert(target.front() == '.');
        dbgAssert(target.back() == '.');

        dbgAssert(!pattern.empty());
        dbgAssert(pattern.front() == '.');
        dbgAssert(pattern.back() == '.');

        return target.ends_with(pattern);
    }
}
