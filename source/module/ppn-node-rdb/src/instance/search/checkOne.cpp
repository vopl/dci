// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "checkOne.hpp"
#include "evaluateBool.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    bool checkOne(query::Scope scope, const pql::Expression& constraints, const table::Record& rec)
    {
        if(query::Scope::online == scope && rec.sv_remote().empty())
        {
            return false;
        }

        if(evaluateBool(rec, constraints))
        {
            return true;
        }

        return false;
    }
}
