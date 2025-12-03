// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "search.hpp"
#include "../instance.hpp"
#include "search/evaluateBool.hpp"
#include "search/checkOne.hpp"

namespace dci::module::ppn::node::rdb::instance
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Search::Search(
            Instance* instance,
            query::Scope scope,
            pql::Expression&& constraints)
        : query::Result<>::Opposite(idl::interface::Initializer{})
        , _instance(instance)
        , _scope(scope)
        , _constraints(std::move(constraints))
    {
        //in start();
        methods()->start() += this * [this]
        {
            startImpl();
        };

        //out next(link::Id, link::Remote);

        //in stop();
        methods()->stop() += this * [this]
        {
            stopImpl();
        };

        //out stopped();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Search::~Search()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::online(const table::Record& rec)
    {
        (void)rec;
        //empty is ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::offline(const table::Record& rec)
    {
        (void)rec;
        //empty is ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::inserted(const table::Record& rec)
    {
        (void)rec;
        //empty is ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::updated(const table::Record& rec)
    {
        (void)rec;
        //empty is ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::remove(const table::Record& rec)
    {
        (void)rec;
        //empty is ok;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::subscribe()
    {
        _instance->searchSubscribe(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::unsubscribe()
    {
        _instance->searchUnsubscribe(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    table::Cursor Search::enumerateRecords()
    {
        return _instance->enumerateRecords();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Search::emitResult(const table::Record& rec)
    {
        //out next(link::Id, link::Remote);
        const Set<link::Remote<>>& remotes = rec.sv_remote();
        methods()->next(rec.sv_id().data, remotes.empty() ? link::Remote<>{} : *remotes.begin());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Search::checkOne(const table::Record& rec)
    {
        return search::checkOne(_scope, _constraints, rec);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Search::startImpl()
    {
        if(_started)
        {
            return false;
        }

        _started = true;
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Search::stopImpl()
    {
        if(!_started)
        {
            return false;
        }

        _started = false;
        methods()->stopped();
        return true;
    }
}
