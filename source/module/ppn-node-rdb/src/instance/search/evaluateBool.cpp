// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "../eval.hpp"
#include "evaluateBool.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    bool evaluateBool(const table::Record& rec, const pql::Expression& expr)
    {
        return eval::cast<bool>(eval::combine(expr, rec).sget<pql::Value>());
    }
}
