// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
