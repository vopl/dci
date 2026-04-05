/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
