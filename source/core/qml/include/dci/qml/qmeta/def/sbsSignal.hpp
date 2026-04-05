/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
