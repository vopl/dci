/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "hook.hpp"
#include <dci/qml/app.hpp>
#include "ep/manager.hpp"

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hook::Hook(QObject* parent)
        : QObject{parent}
    {
        qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
        qRegisterMetaType<QObjectList>("QObjectList");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hook::~Hook()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QAbstractItemModel* Hook::allAsModel(QString tag)
    {
        return ep::g_manager.select(tag)->asModel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObjectList Hook::all(QString tag)
    {
        return ep::g_manager.select(tag)->asArray();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QAbstractItemModel* Hook::oneAsModel(QString tag)
    {
        return ep::g_manager.select(tag, 1)->asModel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* Hook::one(QString tag)
    {
        return ep::g_manager.select(tag, 1)->asObject();
    }
}
