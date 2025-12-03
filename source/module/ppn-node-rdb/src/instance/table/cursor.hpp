// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "record.hpp"

namespace dci::module::ppn::node::rdb::instance::table
{
    class Cursor
        : public Record
    {
    protected:
        Cursor(Table* table, std::size_t index);

    public:
        Cursor() = default;

        explicit operator bool() const;
        void next();
    };
}
