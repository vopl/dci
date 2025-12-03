// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "iterativeMinimize.hpp"
#include "evaluateReal.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    IterativeMinimize::IterativeMinimize(
            Instance* instance,
            query::Scope scope,
            pql::Expression&& constraints,
            pql::Expression&& rate)
        : Search(instance, scope, std::move(constraints))
        , _rate(std::move(rate))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    IterativeMinimize::~IterativeMinimize()
    {
        stopImpl();
        flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool IterativeMinimize::startImpl()
    {
        if(!Search::startImpl())
        {
            return false;
        }

        _currentRate = std::numeric_limits<real64>::max();

        table::Cursor current;
        table::Cursor tc = enumerateRecords();
        while(tc)
        {
            if(checkAndRespectOne(tc))
            {
                current = tc;
            }
            tc.next();
        }

        if(current)
        {
            emitResult(current);
        }

        subscribe();
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool IterativeMinimize::stopImpl()
    {
        if(!Search::stopImpl())
        {
            return false;
        }

        unsubscribe();
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void IterativeMinimize::online(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void IterativeMinimize::offline(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void IterativeMinimize::inserted(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void IterativeMinimize::updated(const table::Record& rec)
    {
        processOne(rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool IterativeMinimize::checkAndRespectOne(const table::Record& rec)
    {
        if(checkOne(rec))
        {
            real64 rate = search::evaluateReal(rec, _rate);

            if(rate < _currentRate)
            {
                _currentRate = rate;
                return true;
            }
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void IterativeMinimize::processOne(const table::Record& rec)
    {
        if(checkAndRespectOne(rec))
        {
            emitResult(rec);
        }
    }
}
