// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/exception/skeleton.hpp>
#include <dci/utils/b2h.hpp>
#include "registry.hpp"
#include <iostream>

#if __has_include(<cxxabi.h>)
#   define HAS_CXXABI 1
#   include <cxxabi.h>
#endif

namespace dci::exception
{
    int registrate(const Eid& eid, const std::type_info& ti, registry::Factory factory)
    {
        registry::Entries& map = registry::map();
        registry::Entries::const_iterator iter = map.find(eid);

        if(iter != map.end())
        {
            const registry::Entry& entry = iter->second;
            if(entry._sign != ti.name())
            {
                std::string sign1;
                std::string sign2;
#if HAS_CXXABI
                int status;
                char* demangled1 = abi::__cxa_demangle(entry._sign.c_str(), nullptr, nullptr, &status);
                char* demangled2 = abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status);

                sign1 = demangled1 ? demangled1 : entry._sign;
                sign2 = demangled2 ? demangled2 : ti.name();

                free(demangled1);
                free(demangled2);
#else
                sign1 = entry._sign;
                sign2 = ti.name();
#endif

                std::cerr<<"duplicated exception eid detected: "<<utils::b2h(eid)<<", "<<sign1<<" vs "<<sign2<<std::endl;

                abort();
            }
            else
            {
                //already registered
                return -1;
            }
        }

        map.emplace(eid, registry::Entry {factory, ti.name()});
        return static_cast<int>(map.size());
    }
}
