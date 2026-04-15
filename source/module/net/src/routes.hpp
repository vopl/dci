/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once
#include "pch.hpp"

namespace dci::module::net
{
    class Host;

    class Routes
        : public sbs::Owner
    {
    public:
        Routes(api::Host<>::Opposite* iface);
        ~Routes();

        void add(const api::route::Entry4& e);
        void del(const api::route::Entry4& e);

        void add(const api::route::Entry6& e);
        void del(const api::route::Entry6& e);

        void flushChanges(bool complete);

    private:
        api::Host<>::Opposite * _iface = nullptr;

        List<api::route::Entry4> _table4;
        List<api::route::Entry6> _table6;

        cmt::Promise<List<api::route::Entry4>>
                                 _table4Initial;
        cmt::Promise<List<api::route::Entry6>>
                                 _table6Initial;

        List<api::route::Entry4> _added4;
        List<api::route::Entry6> _added6;

        List<api::route::Entry4> _deleted4;
        List<api::route::Entry6> _deleted6;
    };
}
