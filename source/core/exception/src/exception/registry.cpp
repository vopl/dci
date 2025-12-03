// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "registry.hpp"

namespace dci::exception::registry
{
    Entries& map()
    {
        static Entries m;
        return m;
    }
}
