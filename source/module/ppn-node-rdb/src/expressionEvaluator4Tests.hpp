// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node::rdb
{
    class ExpressionEvaluator4Tests
        : public pql::ExpressionEvaluator4Tests<>::Opposite
        , public host::module::ServiceBase<ExpressionEvaluator4Tests>
    {
    public:
        ExpressionEvaluator4Tests();
        ~ExpressionEvaluator4Tests();
    };
}
