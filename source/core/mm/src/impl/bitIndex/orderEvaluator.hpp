// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "level.hpp"

namespace dci::mm::impl::bitIndex
{
    template <std::size_t volume, std::size_t base=0>
    struct OrderEvaluator
    {
        struct Current
        {
            static constexpr std::size_t _order = base;
        };

        static constexpr std::size_t _order = std::conditional_t<
                                                volume <= Level<base>::_volume,
                                                Current,
                                                OrderEvaluator<volume, base+1>
                                              >::_order;
    };

}
