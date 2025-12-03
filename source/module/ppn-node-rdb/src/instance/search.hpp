// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "table/cursor.hpp"

namespace dci::module::ppn::node::rdb
{
    class Instance;
}

namespace dci::module::ppn::node::rdb::instance
{
    class Search
        : public query::Result<>::Opposite
        , public sbs::Owner
    {
    public:
        Search(
                Instance* instance,
                query::Scope scope,
                pql::Expression&& constraints);
        virtual ~Search();

        virtual void online(const table::Record& rec);
        virtual void offline(const table::Record& rec);
        virtual void inserted(const table::Record& rec);
        virtual void updated(const table::Record& rec);
        virtual void remove(const table::Record& rec);

    protected:
        void subscribe();
        void unsubscribe();

        table::Cursor enumerateRecords();
        void emitResult(const table::Record& rec);

        bool checkOne(const table::Record& rec);

    protected:
        virtual bool startImpl() = 0;
        virtual bool stopImpl() = 0;

    private:
        Instance *      _instance;
        query::Scope    _scope;
        pql::Expression _constraints;
        bool            _started {false};
    };
}
