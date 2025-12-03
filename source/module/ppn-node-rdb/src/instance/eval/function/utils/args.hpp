// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../../../table/record.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function::utils
{
    class Args
    {
    public:
        Args(const List<pql::Expression>& raw, const table::Record& record);

        const table::Record& record() const;
        std::size_t size() const;
        const pql::Value& operator[](std::size_t idx) const;
        const List<pql::Value>& head(std::size_t amount) const;

    protected:
        const List<pql::Expression>& _raw;
        const table::Record& _record;

        mutable List<pql::Value> _combined;

        static const pql::Value _nullStub;
    };
}
