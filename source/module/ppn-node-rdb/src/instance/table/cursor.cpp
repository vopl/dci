// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "cursor.hpp"
#include "../table.hpp"

namespace dci::module::ppn::node::rdb::instance::table
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(Table* table, std::size_t index)
        : Record(table, index)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::operator bool() const
    {
        if(!_table)
        {
            return false;
        }

        return _index < _table->recordsAmount();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Cursor::next()
    {
        dbgAssert(_table);
        ++_index;
    }
}
