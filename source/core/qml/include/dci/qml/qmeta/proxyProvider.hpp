// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "def.hpp"
#include <type_traits>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class=void>
    requires (Def<T>::_declared)
    struct ProxyProvider
    {
        using Result = T;
    };

    namespace details
    {
        template <class T, class Proxy, class=void>
        struct IsProxyValid
        {
            static constexpr bool _result = false;
        };

        template <class T, class Proxy>
        struct IsProxyValid<T, Proxy, std::void_t<decltype(static_cast<T*>(static_cast<Proxy*>(std::add_pointer_t<T>{})))>>
        {
            static constexpr bool _result = []
            {
#if defined(__GNUC__) && !defined(__clang__)
                union Layout
                {
                    const char    _stub{};
                    const Proxy   _p;
                    const T       _t;

                    constexpr Layout() : _stub{} {}
                    constexpr ~Layout() {}
                };

                constexpr Layout layout;

                return
                    // выравнивание у прокси не менее жесткое чем у таргета
                    alignof(Proxy) >= alignof(T) &&
                    // размер прокси достаточен чтобы содержать таргет
                    sizeof(Proxy) >= sizeof(T) &&
                    // доступность каста уже проверили ранее, теперь проверяем смещение, таргет должен размещаться в самом начале прокси
                    static_cast<const void*>(static_cast<const T*>(&layout._p)) == static_cast<const void*>(&layout._t);
#else
            return
                alignof(Proxy) >= alignof(T) &&
                sizeof(Proxy) >= sizeof(T);
#endif
            }();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    requires (Def<T>::_declared)
    struct ProxyProvider<T, std::void_t<typename Def<T>::Proxy>>
    {
        using Result = typename Def<T>::Proxy;
        static_assert(details::IsProxyValid<T, Result>::_result, "Check Proxy alignment, size and static_casts availability between Proxy* and T*");
    };
}
