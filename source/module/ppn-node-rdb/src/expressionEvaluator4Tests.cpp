// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "expressionEvaluator4Tests.hpp"
#include "instance/eval.hpp"
#include "instance/table.hpp"

namespace dci::module::ppn::node::rdb
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExpressionEvaluator4Tests::ExpressionEvaluator4Tests()
        : pql::ExpressionEvaluator4Tests<>::Opposite(idl::interface::Initializer())
    {
        methods()->evaluate() += serviceSol() * [](const pql::Expression& expr)
        {
            instance::Table tbl(nullptr);
            pql::Expression res = instance::eval::combine(expr, tbl.enumerateRecords());
            return cmt::readyFuture(res.sget<pql::Value>());
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExpressionEvaluator4Tests::~ExpressionEvaluator4Tests()
    {
    }
}
