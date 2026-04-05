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
#include "table/column.hpp"
#include "table/cursor.hpp"

namespace dci::module::ppn::node::rdb
{
    class Instance;
}

namespace dci::module::ppn::node::rdb::instance
{
    class Table
    {
    public:
        Table(Instance* instance);
        ~Table();

        void init(std::vector<List<pql::Column>>&& columnSpecsByFeatures);
        void reset();

        void online(const link::Id& id, const link::Remote<>& remote);
        void offline(const link::Id& id, const link::Remote<>& remote);

        void updateRecord(std::size_t featureIdx, const link::Id& id, List<pql::Value>&& values);

        table::Cursor enumerateRecords();
        table::Record get(const link::Id& id);

    private:
        friend class table::Cursor;
        friend class table::Record;
        std::size_t recordsAmount() const;

        const table::Column<Set<link::Remote<>>>& column4Remote() const;

        const table::Column<pql::Value>* column(std::size_t columnIdx) const;
        const table::Column<pql::Value>* column(const pql::Column& spec) const;

    private:
        table::Record getOrInsert(const link::Id& id, bool* isNew = nullptr);
        void remove(const link::Id& id);

    private:
        Instance*                               _instance;

        //колонки
        table::Column<Set<link::Remote<>>>      _column4Remote;

        std::vector<table::Column<pql::Value>>              _columns;
        std::map<pql::Column, table::Column<pql::Value>*>   _spec2Column;

        using LinkId2RecordIndex = std::map<link::Id, size_t>;
        LinkId2RecordIndex                      _linkId2RecordIndex;

        //расфасовка колонок по фичам
        using ColumnRangesByFeature = std::vector<std::pair<std::size_t, std::size_t>>;
        ColumnRangesByFeature                   _columnRangesByFeature;
    };
}
