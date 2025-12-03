// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include "../api/meth.hpp"
#include "../api/prop.hpp"
#include "../../app.hpp"
#include "../../exception_cast.hpp"
#include <dci/primitives.hpp>
#include <dci/cmt/future.hpp>
#include <QMetaType>
#include <QJSValue>
#include <QQmlApplicationEngine>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T_>
    struct Def<cmt::Future<T_>>
    {
        using T = cmt::Future<T_>;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        static void thenImpl(T& o, sbs::Owner* owner, const QJSValue& func, const QJSValue& instance)
        {
            return o.then() += owner * [func=func, instance=instance](T& in)
            {
                QJSEngine* jse = App::instance()->qengine();

                QJSValue res;
                if(instance.isUndefined())
                {
                    res = func.call(QJSValueList{jse->toScriptValue(in)});
                }
                else
                {
                    res = func.callWithInstance(instance, QJSValueList{jse->toScriptValue(in)});
                }

                if(res.isError())
                {
                    jse->throwError(res);
                    return;
                }
            };
        }

        static cmt::Future<QJSValue> chainImpl(T& o, sbs::Owner* owner, const QJSValue& func, const QJSValue& instance)
        {
            return o.chain() += owner * [func=func, instance=instance](T& in, cmt::Promise<QJSValue>& out)
            {
                if(in.resolvedCancel())
                {
                    out.resolveCancel();
                    return;
                }

                if(in.resolvedException())
                {
                    out.resolveException(in.exception());
                    return;
                }

                QJSEngine* jse = App::instance()->qengine();

                QJSValue res;
                if(instance.isUndefined())
                {
                    res = func.call(QJSValueList{jse->toScriptValue(in)});
                }
                else
                {
                    res = func.callWithInstance(instance, QJSValueList{jse->toScriptValue(in)});
                }

                if(res.isError())
                {
                    out.resolveException(exception_cast<ExceptionPtr>(res));
                }
                else
                {
                    out.resolveValue(res);
                }
            };
        }

        using Api = TList
        <
            api::PropR<"charged", [](const T& o) { return o.charged(); }>,
            api::PropR<"resolved", [](const T& o) { return o.resolved(); }>,
            api::PropR<"resolvedValue", [](const T& o) { return o.resolvedValue(); }>,
            api::PropR<"resolvedException", [](const T& o) { return o.resolvedException(); }>,
            api::PropR<"resolvedCancel", [](const T& o) { return o.resolvedCancel(); }>,
            api::PropR<"exception", [](const T& o)
            {
                if(o.resolvedException()) return o.exception();
                return decltype(o.exception()){};
            }>,
            api::PropR<"value", [](const T& o)
            {
                if constexpr(T::_isVoid)
                {
                    if(o.resolvedValue()) return QJSValue{};
                    return QJSValue{};
                }
                else
                {
                    if(o.resolvedValue()) return o.value();
                    return decltype(o.value()){};
                }
            }>,

            api::Meth<"cancel", [](T& o) { return o.resolveCancel(); }>,
            api::Meth<"uncharge", [](T& o) { return o.uncharge(); }>,

            api::Meth<"then", [](T& o, sbs::Owner& owner, const QJSValue& func, const QJSValue& instance) { return thenImpl(o, &owner,  func, instance); }, "owner", "func", "instance">,
            api::Meth<"then", [](T& o, sbs::Owner& owner, const QJSValue& func                          ) { return thenImpl(o, &owner,  func, {}      ); }, "owner", "func"            >,
            api::Meth<"then", [](T& o,                    const QJSValue& func, const QJSValue& instance) { return thenImpl(o, nullptr, func, instance); },          "func", "instance">,
            api::Meth<"then", [](T& o,                    const QJSValue& func                          ) { return thenImpl(o, nullptr, func, {}      ); },          "func"            >,

            api::Meth<"chain", [](T& o, sbs::Owner& owner, const QJSValue& func, const QJSValue& instance) { return chainImpl(o, &owner,  func, instance); }, "owner", "func", "instance">,
            api::Meth<"chain", [](T& o, sbs::Owner& owner, const QJSValue& func                          ) { return chainImpl(o, &owner,  func, {}      ); }, "owner", "func"            >,
            api::Meth<"chain", [](T& o,                    const QJSValue& func, const QJSValue& instance) { return chainImpl(o, nullptr, func, instance); },          "func", "instance">,
            api::Meth<"chain", [](T& o,                    const QJSValue& func                          ) { return chainImpl(o, nullptr, func, {}      ); },          "func"            >
        >;
    };
}
