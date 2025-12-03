// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
