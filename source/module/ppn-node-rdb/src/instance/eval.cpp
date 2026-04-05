/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "eval.hpp"

#include "eval/function/cmn.hpp"
#include "eval/function/num.hpp"
#include "eval/function/numseq.hpp"
#include "eval/function/bool.hpp"
#include "eval/function/bits.hpp"
#include "eval/function/str.hpp"
#include "eval/function/id.hpp"
#include "eval/function/seq.hpp"

#include "table/record.hpp"

namespace dci::module::ppn::node::rdb::instance::eval
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    static pql::Expression combine(const pql::Node& node, const table::Record& record)
    {
        function::Args args(node.args, record);

        switch(node.op)
        {
#define CASE_ONE(V) case pql::fun::V: return function::exec<pql::fun::V>(args);

        CASE_ONE(cmn_eqType)
        CASE_ONE(cmn_eq)
        CASE_ONE(cmn_ne)
        CASE_ONE(cmn_gt)
        CASE_ONE(cmn_lt)
        CASE_ONE(cmn_ge)
        CASE_ONE(cmn_le)
        CASE_ONE(num_eq)
        CASE_ONE(num_ne)
        CASE_ONE(num_gt)
        CASE_ONE(num_lt)
        CASE_ONE(num_ge)
        CASE_ONE(num_le)
        CASE_ONE(num_between)
        CASE_ONE(num_neg)
        CASE_ONE(num_add)
        CASE_ONE(num_sub)
        CASE_ONE(num_div)
        CASE_ONE(num_mul)
        CASE_ONE(num_mod)
        CASE_ONE(num_ceil)
        CASE_ONE(num_floor)
        CASE_ONE(num_trunc)
        CASE_ONE(num_round)
        CASE_ONE(num_abs)
        CASE_ONE(num_log)
        CASE_ONE(num_log10)
        CASE_ONE(num_log2)
        CASE_ONE(num_exp)
        CASE_ONE(num_exp10)
        CASE_ONE(num_exp2)
        CASE_ONE(num_pow)
        CASE_ONE(num_sqrt)
        CASE_ONE(num_cbrt)
        CASE_ONE(num_hypot)
        CASE_ONE(num_sin)
        CASE_ONE(num_cos)
        CASE_ONE(num_tan)
        CASE_ONE(num_asin)
        CASE_ONE(num_acos)
        CASE_ONE(num_atan)
        CASE_ONE(num_atan2)
        CASE_ONE(numseq_max)
        CASE_ONE(numseq_min)
        CASE_ONE(numseq_avg)
        CASE_ONE(numseq_median)
        CASE_ONE(numseq_sum)
        CASE_ONE(bool_not)
        CASE_ONE(bool_and)
        CASE_ONE(bool_or)
        CASE_ONE(bool_xor)
        CASE_ONE(bits_neg)
        CASE_ONE(bits_and)
        CASE_ONE(bits_or)
        CASE_ONE(bits_xor)
        CASE_ONE(str_concat)
        CASE_ONE(str_find)
        CASE_ONE(str_match)
        CASE_ONE(str_startsWith)
        CASE_ONE(str_endsWith)
        CASE_ONE(str_substr)
        CASE_ONE(id_tip8)
        CASE_ONE(id_tip16)
        CASE_ONE(id_tip32)
        CASE_ONE(id_tip64)
        CASE_ONE(seq_map)
        CASE_ONE(seq_grep)
        CASE_ONE(seq_reduce)
        CASE_ONE(seq_at)
        CASE_ONE(seq_size)

#undef CASE_ONE
        }

        return pql::Expression{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    pql::Expression combine(const pql::Expression& expr, const table::Record& record)
    {
        return expr.visit([&](const auto& v) -> pql::Expression
        {
            using V = std::decay_t<decltype(v)>;
            if constexpr(std::is_same_v<pql::Node, V>)
            {
                return combine(v, record);
            }
            if constexpr(std::is_same_v<pql::Value, V>)
            {
                return v;
            }
            if constexpr(std::is_same_v<pql::Column, V>)
            {
                return record.value(v);
            }
        });
    }
}
