// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/exception/buildInstance.hpp>
#include "registry.hpp"

namespace dci::exception
{
    std::exception_ptr buildInstance(const Eid& eid, const std::exception_ptr& cause)
    {
        auto iter = registry::map().find(eid);
        if(registry::map().end() == iter)
        {
            return std::exception_ptr();
        }

        return iter->second._factory(cause);
    }
}
