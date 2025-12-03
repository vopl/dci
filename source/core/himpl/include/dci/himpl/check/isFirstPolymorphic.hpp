// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <type_traits>

namespace dci::himpl::check
{
    template <class... T>
    struct IsFirstPolymorphic
    {
        static bool constexpr _value = false;
    };

    template <class TFirst, class... T>
    struct IsFirstPolymorphic<TFirst, T...>
    {
        static bool constexpr _value = std::is_polymorphic<TFirst>::value;
    };

}
