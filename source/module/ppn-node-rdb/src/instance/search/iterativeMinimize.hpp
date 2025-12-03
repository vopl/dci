// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../search.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    class IterativeMinimize final
        : public Search
        , public mm::heap::Allocable<IterativeMinimize>
    {
    public:
        IterativeMinimize(
                Instance* instance,
                query::Scope scope,
                pql::Expression&& constraints,
                pql::Expression&& rate);
        ~IterativeMinimize() override;

    private:
        bool startImpl() override;
        bool stopImpl() override;

        void online(const table::Record& rec) override;
        void offline(const table::Record& rec) override;
        void inserted(const table::Record& rec) override;
        void updated(const table::Record& rec) override;

    private:
        bool checkAndRespectOne(const table::Record& rec);
        void processOne(const table::Record& rec);

    private:
        pql::Expression    _rate;
        real64             _currentRate {std::numeric_limits<real64>::max()};
    };
}
