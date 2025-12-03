// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include "../api/meth.hpp"
#include "../api/prop.hpp"
#include "../../app.hpp"
#include "../../future_cast.hpp"
#include <dci/sbs/signal.hpp>
#include <QMetaType>
#include <QJSValue>
#include <QQmlApplicationEngine>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    struct Def<sbs::Signal<R, Args...>>
    {
        using T = sbs::Signal<R, Args...>;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        static auto connectImpl(T& o, sbs::Owner* owner, const QJSValue& func, const QJSValue& instance)
        {
            auto cb = [func=func, instance=instance](auto&&... args)
            {
                QJSEngine* jse = App::instance()->qengine();
                QJSValue res;
                if(jse)
                {
                    if(instance.isUndefined())
                    {
                        res = func.call(QJSValueList{jse->toScriptValue(args)...});
                    }
                    else
                    {
                        res = func.callWithInstance(instance, QJSValueList{jse->toScriptValue(args)...});
                    }

                    if(res.isError())
                    {
                        jse->throwError(res);
                        res = {};
                    }
                }
                else
                {
                    qWarning() << "no qml::App available, unable to call QML signal reactor for: " << _name._buf;
                }

                if constexpr(std::is_same_v<void, R>)
                {
                    (void)res;
                    return;
                }
                else
                {
                    return future_cast<R>(res);
                }
            };

            if(owner)
                return o += owner * cb;

            return o += cb;
        }

        using Api = TList
        <
            api::Meth<"connect", [](T& o, sbs::Owner& owner, const QJSValue& func, const QJSValue& instance) { return connectImpl(o, &owner,  func, instance); }, "owner", "func", "instance">,
            api::Meth<"connect", [](T& o, sbs::Owner& owner, const QJSValue& func                          ) { return connectImpl(o, &owner,  func, {}      ); }, "owner", "func"            >,
            api::Meth<"connect", [](T& o,                    const QJSValue& func, const QJSValue& instance) { return connectImpl(o, nullptr, func, instance); },          "func", "instance">,
            api::Meth<"connect", [](T& o,                    const QJSValue& func                          ) { return connectImpl(o, nullptr, func, {}      ); },          "func"            >
        >;
    };
}
