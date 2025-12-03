// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../table/record.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    real64 evaluateReal(const table::Record& rec, const pql::Expression& expr);
}
