// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <type_traits>

namespace dci::idl::contract
{
    template<typename A, typename T, int seed=0>
    concept Arg =
            std::is_constructible_v<T, A> ||
            std::is_convertible_v<A, T>;
}
