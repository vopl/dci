// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "../eval.hpp"
#include "evaluateReal.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    real64 evaluateReal(const table::Record& rec, const pql::Expression& expr)
    {
        return eval::cast<real64>(eval::combine(expr, rec).sget<pql::Value>());
    }
}
