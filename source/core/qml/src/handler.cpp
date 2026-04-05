/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "handler.hpp"

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Handler::Handler(QObject* parent)
        : QObject{parent}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Handler::~Handler()
    {
        _connectionsOwner.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handler::setTarget(const QQmlProperty &prop)
    {
        if(prop == _targetProperty)
        {
            return;
        }

        if(_targetProperty.isValid())
        {
            dbgAssert(_targetProperty.object());
            if(_targetProperty.object())
            {
                QObject::disconnect(_targetProperty.object(), nullptr, this, SLOT(update()));
            }
        }

        _targetProperty = prop;

        if(_targetProperty.isValid())
        {
            _targetProperty.connectNotifySignal(this, SLOT(update()));
        }

        update();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handler::classBegin()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handler::componentComplete()
    {
        const QMetaObject* metaObject = this->metaObject();// это тип QML-ного объекта
        QJSEngine* jse = qjsEngine(this);
        QJSValue jsSelf = jse->toScriptValue(this);
        for(int i{}; i<metaObject->methodCount(); ++i)
        {
            QMetaMethod metaMethod = metaObject->method(i);
            if(metaMethod.enclosingMetaObject() != metaObject)
            {
                continue;
            }

            if(!jsSelf.hasOwnProperty(metaMethod.name()))
            {
                continue;
            }

            QJSValue jsMethod = jsSelf.property(metaMethod.name());
            if(!jsMethod.isCallable())
            {
                continue;
            }

            _reactors << Reactor{metaMethod, jsMethod};
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QVariant Handler::getTarget() const
    {
        return _target;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handler::setTarget(const QVariant& v)
    {
        if(_target == v)
        {
            return;
        }

        _target = v;
        update();
        emit targetChanged();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Handler::getEnabled() const
    {
        return _enabled;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handler::setEnabled(bool v)
    {
        if(_enabled == v)
        {
            return;
        }

        _enabled = v;
        update();
        emit enabledChanged();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Handler::getCharged() const
    {
        return _charged;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Handler::getInvolved() const
    {
        return _involved;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Handler::update()
    {
        _connectionsOwner.flush();

        bool involved = false;
        bool charged = false;
        utils::AtScopeExit finalize{[&,this]
        {
            if(_charged != charged)
            {
                _charged = charged;
                emit this->chargedChanged();
            }
            if(_involved != involved)
            {
                _involved = involved;
                emit this->involvedChanged();
            }
        }};

        if(!_enabled)
        {
            return;
        }

        QVariant target = _target.isValid() ? _target : _targetProperty.read();
        dci::idl::Interface interface = target.value<dci::idl::Interface>();
        if(!interface)
        {
            return;
        }

        charged = true;
        involved = interface.involved();
        interface.involvedChanged() += _connectionsOwner * [this](bool v)
        {
            if(_involved != v)
            {
                _involved = v;
                emit this->involvedChanged();
            }
        };

        QMetaType targetMetaType = target.metaType();
        if(!(targetMetaType.flags() & QMetaType::IsGadget))
        {
            qWarning() << "target isnt a Gadget: " << targetMetaType.name();
            return;
        }

        const QMetaObject* targetMetaObject = targetMetaType.metaObject();
        if(!targetMetaObject)
        {
            qWarning() << "target has no QMetaObject: " << targetMetaType.name();
            return;
        }

        QJSEngine* jse = qjsEngine(this);

        for(const auto& [reactorMeta, reactorJs] : _reactors)
        {
            const QByteArray sname = reactorMeta.name();
            int targetPropIdx = targetMetaObject->indexOfProperty(sname.constData());
            if (0 > targetPropIdx)
            {
                qWarning() << "signal " << sname << " isnt found in target: " << targetMetaObject->className();
                continue;
            }
            QMetaProperty signalProperty = targetMetaObject->property(targetPropIdx);
            QVariant signal = signalProperty.readOnGadget(target.constData());

            QMetaType signalMetaType = signal.metaType();
            const QMetaObject* signalMetaObject = signalMetaType.metaObject();

            if(!signalMetaObject)
            {
                qWarning() << "signal " << sname << " has no metaObject";
                continue;
            }

            int signalConnectIdx = signalMetaObject->indexOfMethod("connect(dci::sbs::Owner,QJSValue,QJSValue)");
            if (0 > signalConnectIdx)
            {
                qWarning() << "method 'connect(dci::sbs::Owner,QJSValue,QJSValue)' isnt found in signal: " << signalMetaObject->className();
                continue;
            }

            QMetaMethod signalMetaMethod = signalMetaObject->method(signalConnectIdx);
            bool b = signalMetaMethod.invokeOnGadget(
                        signal.data(),
                        QArgument<dci::sbs::Owner>{QMetaType::fromType<dci::sbs::Owner>().name(), _connectionsOwner},
                        QArgument<QJSValue>{QMetaType::fromType<QJSValue>().name(), reactorJs},
                        QArgument<QJSValue>{QMetaType::fromType<QJSValue>().name(), jse->toScriptValue(this)});

            dbgAssertX(b, "signal.invokeOnGadget in qml Handler");
            (void)b;
        }
    }
}
