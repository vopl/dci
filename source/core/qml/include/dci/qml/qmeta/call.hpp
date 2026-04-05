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
#include "api/prop.hpp"
#include "api/meth.hpp"
#include "api/ctor.hpp"
#include "proxyProvider.hpp"
#include <QMetaObject>

namespace dci::qml::qmeta
{
    template <class T>
    requires (Def<T>::_declared)
    struct Call
    {
        using GD = Def<T>;
        using Proxy = typename ProxyProvider<T>::Result;

        using Props = typename GD::Api::template Grep<api::IsProp>;
        using Meths = typename GD::Api::template Grep<api::IsMeth>;
        using Ctors = typename GD::Api::template Grep<api::IsCtor>;

        template <class Api>
        struct Invoke
        {
            static void exec(Proxy* o, void** a)
            {
                auto bindedInvokeImpl = [&]() -> decltype(auto)
                {
                    return [&]<class... Param, auto... idx>(TList<Param...>, VList<idx...>) -> decltype(auto)
                    {
                        return Api::_invoke(*o, *reinterpret_cast<Param*>(a[1+idx])...);
                    }(typename Api::ParamTypes{}, MakeSeq<Api::ParamTypes::_size>{});
                };

                if constexpr (std::is_same_v<void, typename Api::Return>)
                {
                    bindedInvokeImpl();
                }
                else
                {
                    if(a[0])
                    {
                        *reinterpret_cast<typename Api::Return*>(a[0]) = bindedInvokeImpl();
                    }
                    else
                    {
                        (void)bindedInvokeImpl();
                    }
                }
            }
        };

        template <class Api>
        struct Read
        {
            static void exec(Proxy* o, void** a)
            {
                if constexpr(Api::_isPropR)
                {
                    *reinterpret_cast<typename Api::Type*>(a[0]) = Api::_read(*o);
                }
            }
        };

        template <class Api>
        struct Write
        {
            static void exec(Proxy* o, void** a)
            {
                if constexpr(Api::_isPropW)
                {
                    Api::_write(*o, *reinterpret_cast<typename Api::Type*>(a[0]));
                }
            }
        };

        template <class L, template<class> class Adaptor>
        static constexpr void activate(int id, QObject* o, void** a)
        {
            [&]<class... Api>(TList<Api...>)
            {
                if constexpr(sizeof...(Api))
                {
                    dbgAssert(id >= 0);
                    dbgAssert(static_cast<std::size_t>(id) < sizeof...(Api));

                    void (*arr[])(Proxy*, void**) = {Adaptor<Api>::exec...};
                    dbgAssert(arr[id]);

                    arr[id](reinterpret_cast<Proxy*>(o), a);
                }
            }(L{});
        }

        static constexpr void staticMetacallFunction(QObject* o, QMetaObject::Call c, int id, void** a)
        {
            switch(c)
            {
            case QMetaObject::CreateInstance:
                activate<Ctors, Invoke>(id, o, a);
                break;
            case QMetaObject::InvokeMetaMethod:
                activate<Meths, Invoke>(id, o, a);
                break;
            case QMetaObject::ReadProperty:
                activate<Props, Read>(id, o, a);
                break;
            case QMetaObject::WriteProperty:
                activate<Props, Write>(id, o, a);
                break;
            default:
                dbgFatal("not impl");
                break;
            }
        }

        //////////////////////////////////////////
        static constexpr QMetaObject::Data::StaticMetacallFunction qt()
        {
            return &staticMetacallFunction;
        }
    };
}
