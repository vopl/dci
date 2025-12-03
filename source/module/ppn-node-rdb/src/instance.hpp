// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "instance/table.hpp"
#include "instance/search.hpp"
#include "instance/table/record.hpp"

namespace dci::module::ppn::node::rdb
{
    class Instance
        : public api::Instance<>::Opposite
        , public sbs::Owner
        , public mm::heap::Allocable<Instance>
    {
    public:
        Instance();
        ~Instance();

    public:
        cmt::Future<void> initialize(List<api::Feature<>>&& features);

    public:
        void searchSubscribe(instance::Search* s);
        void searchUnsubscribe(instance::Search* s);

        instance::table::Cursor enumerateRecords();

    public:
        void online(const instance::table::Record& rec);
        void offline(const instance::table::Record& rec);
        void inserted(const instance::table::Record& rec);
        void updated(const instance::table::Record& rec);
        void remove(const instance::table::Record& rec);

    private:
        api::feature::Service<>::Opposite   _featureService;
        instance::Table                     _table;
        std::set<instance::Search*>         _searches;
    };
}
