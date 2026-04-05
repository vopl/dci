/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "table.hpp"
#include "../instance.hpp"

namespace dci::module::ppn::node::rdb::instance
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Table::Table(Instance* instance)
        : _instance(instance)
        , _column4Remote(pql::Column{})
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Table::~Table()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Table::init(std::vector<List<pql::Column>>&& columnSpecsByFeatures)
    {
        reset();

        //_column4Remote already exists

        _columns.emplace_back(pql::Column{idl::ILid{}, "id"});
        _columnRangesByFeature.emplace_back(std::make_pair(0, 1));

        for(std::size_t featureIdx(0); featureIdx<columnSpecsByFeatures.size(); ++featureIdx)
        {
            std::size_t begin = _columns.size();

            const auto& featureColumns = columnSpecsByFeatures[featureIdx];
            for(std::size_t j(0); j<featureColumns.size(); ++j)
            {
                const auto& featureColumn = featureColumns[j];
                _columns.emplace_back(featureColumn);
            }

            std::size_t end = _columns.size();

            _columnRangesByFeature.emplace_back(std::make_pair(begin, end));
        }

        for(table::Column<pql::Value>& c : _columns)
        {
            _spec2Column.insert(std::make_pair(c.spec(), &c));
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Table::reset()
    {
        _columns.clear();
        _columnRangesByFeature.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Table::online(const link::Id& id, const link::Remote<>& remote)
    {
        table::Record record = getOrInsert(id);

        Set<link::Remote<>>& remotes = _column4Remote.access(record.index());
        remotes.insert(remote);
        if(1 == remotes.size())
        {
            _instance->online(record);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Table::offline(const link::Id& id, const link::Remote<>& remote)
    {
        (void)remote;

        table::Record record = getOrInsert(id);

        Set<link::Remote<>>& remotes = _column4Remote.access(record.index());
        remotes.erase(remote);

        if(remotes.empty())
        {
            _instance->offline(record);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Table::updateRecord(std::size_t featureIdx, const link::Id& id, List<pql::Value>&& values)
    {
        bool isNew;
        table::Record record = getOrInsert(id, &isNew);

        //update
        dbgAssert(featureIdx+1 < _columnRangesByFeature.size());
        const auto& columnsRange = _columnRangesByFeature[featureIdx+1];
        dbgAssert(values.size() == columnsRange.second - columnsRange.first);
        for(std::size_t columnIdx(columnsRange.first); columnIdx<columnsRange.second; ++columnIdx)
        {
            _columns[columnIdx].set(record.index(), std::move(values[columnIdx - columnsRange.first]));
        }
        values.clear();

        if(isNew)
        {
            _instance->inserted(record);
        }
        else
        {
            _instance->updated(record);
        }
    }

    namespace
    {
        struct CursorBuilder : table::Cursor
        {
            CursorBuilder(Table* table, std::size_t index)
                : table::Cursor{table, index}
            {
            }
        };

        struct RecordBuilder : table::Record
        {
            RecordBuilder(Table* table, std::size_t index)
                : table::Record{table, index}
            {
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    table::Cursor Table::enumerateRecords()
    {
        return CursorBuilder{this, 0};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    table::Record Table::get(const link::Id& id)
    {
        auto iter = _linkId2RecordIndex.find(id);
        if(_linkId2RecordIndex.end() == iter)
        {
            return table::Record{};
        }

        return RecordBuilder{this, iter->second};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Table::recordsAmount() const
    {
        return _column4Remote.recordsAmount();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const table::Column<Set<link::Remote<>>>& Table::column4Remote() const
    {
        return _column4Remote;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const table::Column<pql::Value>* Table::column(std::size_t columnIdx) const
    {
        dbgAssert(_columns.size() > columnIdx);
        return &_columns[columnIdx];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const table::Column<pql::Value>* Table::column(const pql::Column& spec) const
    {
        auto iter = _spec2Column.find(spec);
        if(_spec2Column.end() == iter)
        {
            return nullptr;
        }

        return iter->second;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    table::Record Table::getOrInsert(const link::Id& id, bool* isNew)
    {
        dbgAssert(_linkId2RecordIndex.size() == recordsAmount());

        auto ip = _linkId2RecordIndex.insert(std::make_pair(id, _linkId2RecordIndex.size()));
        std::size_t recordIdx = ip.first->second;

        if(ip.second)
        {
            _column4Remote.insert();

            for(table::Column<pql::Value>& c : _columns)
            {
                dbgAssert(c.recordsAmount() == recordIdx);
                c.insert();
                dbgAssert(c.recordsAmount() == recordIdx+1);
            }

            _columns[0].set(recordIdx, pql::Value{pql::id256{id}});
        }

        if(isNew)
        {
            *isNew = ip.second;
        }

        dbgAssert(pql::Value{pql::id256{id}} == _columns[0].get(recordIdx));

        return RecordBuilder(this, recordIdx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Table::remove(const link::Id& id)
    {
        dbgAssert(_linkId2RecordIndex.size() == recordsAmount());

        auto iter = _linkId2RecordIndex.find(id);

        if(_linkId2RecordIndex.end() == iter)
        {
            return;
        }

        std::size_t recordIdx = iter->second;
        (--_linkId2RecordIndex.end())->second = recordIdx;

        dbgAssert(recordIdx < recordsAmount());
        for(table::Column<pql::Value>& c : _columns)
        {
            c.remove(recordIdx);
        }
        _column4Remote.remove(recordIdx);
    }
}
