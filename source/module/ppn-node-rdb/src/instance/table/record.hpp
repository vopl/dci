// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node::rdb::instance
{
    class Table;
}

namespace dci::module::ppn::node::rdb::instance::table
{
    class Record
    {
    protected:
        Record(Table* table, std::size_t index);

    public:
        Record();
        Record(const Record& from);
        ~Record();

        Record& operator=(const Record& from);

        std::size_t index() const;

        const Set<link::Remote<>>& sv_remote() const;
        const pql::id256& sv_id() const;

        const pql::Value& value(const pql::Column& c) const;

    protected:
        Table *     _table {nullptr};
        std::size_t _index {std::numeric_limits<std::size_t>::max()};
    };
}
