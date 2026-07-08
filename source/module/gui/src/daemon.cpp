/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "daemon.hpp"
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlApplicationEngine>

extern "C"
{
    extern dci::host::module::Entry* dciModuleEntry;
}

namespace dci::module::gui
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Daemon::Daemon()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Daemon::~Daemon()
    {
        _app.reset();
        _epExtensionInstance.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Daemon::startImpl(idl::gen::Config&& config)
    {
        (void)config;

        dbgAssert(!_app);
        _app = qml::App::instance();


        dbgAssert(!_qmlContext);
        _qmlContext = new QQmlContext{_app->qengine(), static_cast<QObject*>(_app->qengine())};

        dbgAssert(!_epExtensionInstance);
        _epExtensionInstance = qml::ep::uniqueExtension([this]()
        {
            dbgAssert(_app);
            dbgAssert(_qmlContext);

            QQmlComponent* c = new QQmlComponent
            {
                _app->qengine(),
                QString{"../module/gui/qml/Entry.qml"},
                QQmlComponent::PreferSynchronous
            };

            _app->qengine()->setContextForObject(c, _qmlContext);

            return c;
        }, "entry");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Daemon::stopImpl()
    {
        _epExtensionInstance.reset();
        _app.reset();
        dbgAssert(!_qmlContext);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    idl::Interface Daemon::serviceImpl()
    {
        return idl::Interface{};
    }
}
