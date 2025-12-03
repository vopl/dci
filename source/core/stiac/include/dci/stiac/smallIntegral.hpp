// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <type_traits>

namespace dci::stiac
{
    template <class T> requires std::is_integral_v<T>
    struct SmallIntegral
    {
        static constexpr bool _isSmallIntegral = true;
        static constexpr bool _isSigned = std::is_signed_v<T>;
        static constexpr bool _isUnsigned = std::is_unsigned_v<T>;

        using V = T;
        V _v;
    };

    template <class T> requires std::is_integral_v<T>
    const SmallIntegral<T>& smallIntegral(const T& v)
    {
        static_assert(sizeof(T) == sizeof(SmallIntegral<T>));
        static_assert(alignof(T) == alignof(SmallIntegral<T>));

        return *static_cast<const SmallIntegral<T>*>(static_cast<const void*>(&v));
    }

    template <class T> requires std::is_integral_v<T>
    SmallIntegral<T>& smallIntegral(T& v)
    {
        static_assert(sizeof(T) == sizeof(SmallIntegral<T>));
        static_assert(alignof(T) == alignof(SmallIntegral<T>));

        return *static_cast<SmallIntegral<T>*>(static_cast<void*>(&v));
    }

    template <class T> requires std::is_integral_v<T>
    SmallIntegral<T>&& smallIntegral(T&& v)
    {
        static_assert(sizeof(T) == sizeof(SmallIntegral<T>));
        static_assert(alignof(T) == alignof(SmallIntegral<T>));

        return std::move(*static_cast<SmallIntegral<T>*>(static_cast<void*>(&v)));
    }
}
