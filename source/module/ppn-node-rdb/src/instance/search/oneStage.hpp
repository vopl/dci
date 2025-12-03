// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../search.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    class OneStage final
        : public Search
        , public mm::heap::Allocable<OneStage>
    {
    public:
        OneStage(
                Instance* instance,
                query::Scope scope,
                pql::Expression&& constraints);
        ~OneStage() override;

    private:
        bool startImpl() override;
        bool stopImpl() override;
    };
}
