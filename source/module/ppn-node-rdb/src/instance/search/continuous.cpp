// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "continuous.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Continuous::Continuous(
            Instance* instance,
            query::Scope scope,
            pql::Expression&& constraints)
        : Search(instance, scope, std::move(constraints))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Continuous::~Continuous()
    {
        stopImpl();
        flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Continuous::startImpl()
    {
        if(!Search::startImpl())
        {
            return false;
        }

        table::Cursor tc = enumerateRecords();
        while(tc)
        {
            processOne(tc);
            tc.next();
        }

        subscribe();
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Continuous::stopImpl()
    {
        if(!Search::stopImpl())
        {
            return false;
        }

        unsubscribe();
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Continuous::online(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Continuous::offline(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Continuous::inserted(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Continuous::updated(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Continuous::processOne(const table::Record& rec)
    {
        if(checkOne(rec))
        {
            emitResult(rec);
        }
    }
}
