// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstddef>
#include <type_traits>

namespace dci::himpl::details
{
    template <class Size, class Entropy>
    struct Space
    {
        char _space[Size::value];
    };

    template <class Entropy>
    struct Space<std::integral_constant<std::size_t, 0>, Entropy>
    {
    };
}
