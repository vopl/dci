// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "record.hpp"
#include "../table.hpp"

namespace dci::module::ppn::node::rdb::instance::table
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Record::Record(Table* table, std::size_t index)
        : _table(table)
        , _index(index)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Record::Record()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Record::Record(const Record& from)
        : _table(from._table)
        , _index(from._index)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Record::~Record()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Record& Record::operator=(const Record& from)
    {
        if(this != &from)
        {
            _table = from._table;
            _index = from._index;
        }

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Record::index() const
    {
        return _index;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Set<link::Remote<>>& Record::sv_remote() const
    {
        return _table->column4Remote().get(_index);
    }

    namespace
    {
        const pql::id256 g_nullId256Stub {};
        const pql::Value g_nullValueStub {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const pql::id256& Record::sv_id() const
    {
        return _table->column(0)->get(_index).data.getOr<pql::id256>(g_nullId256Stub);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const pql::Value& Record::value(const pql::Column& c) const
    {
        if(!_table)
        {
            return g_nullValueStub;
        }

        const table::Column<pql::Value>* tc = _table->column(c);
        if(!tc)
        {
            return g_nullValueStub;
        }

        return tc->get(_index);
    }

}
