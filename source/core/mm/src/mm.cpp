// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/mm.hpp>
#include "impl/virtualSpace.hpp"

namespace dci::mm
{
    void setupPanicHandler(void(* panic)(int))
    {
        impl::VirtualSpace::single().setupPanicHandler(panic);
    }
}
