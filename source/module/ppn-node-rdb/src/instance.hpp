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
#include "instance/table.hpp"
#include "instance/search.hpp"
#include "instance/table/record.hpp"

namespace dci::module::ppn::node::rdb
{
    class Instance
        : public api::Instance<>::Opposite
        , public sbs::Owner
        , public mm::heap::Allocable<Instance>
    {
    public:
        Instance();
        ~Instance();

    public:
        cmt::Future<void> initialize(List<api::Feature<>>&& features);

    public:
        void searchSubscribe(instance::Search* s);
        void searchUnsubscribe(instance::Search* s);

        instance::table::Cursor enumerateRecords();

    public:
        void online(const instance::table::Record& rec);
        void offline(const instance::table::Record& rec);
        void inserted(const instance::table::Record& rec);
        void updated(const instance::table::Record& rec);
        void remove(const instance::table::Record& rec);

    private:
        api::feature::Service<>::Opposite   _featureService;
        instance::Table                     _table;
        std::set<instance::Search*>         _searches;
    };
}
