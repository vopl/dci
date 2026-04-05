/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include <dci/qml/ep.hpp>
#include "ep/manager.hpp"

namespace dci::qml::ep
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Extension::~Extension()
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Extension::startup(App *)
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Extension::shutdown(App *)
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* Extension::activate(App *, const QString&)
    {
        return nullptr;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void registrate(Extension* e, const QString& tag, int priority)
    {
        g_manager.registrate(e, tag, priority);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void deregistrate(Extension* e)
    {
        g_manager.deregistrate(e);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void deregistrate(Extension* e, const QString& tag)
    {
        g_manager.deregistrate(e, tag);
    }
}
