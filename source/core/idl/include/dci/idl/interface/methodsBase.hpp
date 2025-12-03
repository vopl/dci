// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "side.hpp"
#include "stateHolder.hpp"

namespace dci::idl::interface
{
    template <template<Side> class C, Side s>
    struct MethodsBase
    {
        template <template <Side> class, Side>
        friend struct Methods;

        template <template <Side> class, Side>
        friend class ImplBase;

        template <bool>
        friend class Generic;

    private:
        StateHolder<C, s> _stateHolder;
    };
}
