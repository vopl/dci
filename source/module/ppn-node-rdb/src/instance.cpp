// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "instance.hpp"
#include "instance/search/continuous.hpp"
#include "instance/search/iterativeMinimize.hpp"
#include "instance/search/oneStage.hpp"
#include "instance/search/checkOne.hpp"

namespace dci::module::ppn::node::rdb
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::Instance()
        : api::Instance<>::Opposite(idl::interface::Initializer())
        , _featureService(idl::interface::Initializer())
        , _table(this)
    {
    //    in addRemote(link::Id, link::Remote);
        methods()->addRemote() += this * [this](const link::Id& id, link::Remote<> remote)
        {
            remote->closed() += this * [this, remote=remote.weak(), id]
            {
                _table.offline(id, remote);
            };

            _table.online(id, remote);
        };

    //    in iterativeMinimize(
    //        query::Scope scope,
    //        pql::Expression constraints,
    //        pql::Expression rate
    //    ) -> query::Result;
        _featureService->iterativeMinimize() += this * [this](
                                query::Scope scope,
                                pql::Expression&& constraints,
                                pql::Expression&& rate)
        {
            auto* res = new instance::search::IterativeMinimize(this, scope, std::move(constraints), std::move(rate));
            res->involvedChanged() += res * [res](bool v)
            {
                if(!v)
                {
                    delete res;
                }
            };

            return cmt::readyFuture(res->opposite());
        };

    //    in continuous(
    //        query::Scope scope,
    //        pql::Expression constraints
    //    ) -> query::Result;
        _featureService->continuous() += this * [this](
                                query::Scope scope,
                                pql::Expression&& constraints)
        {
            auto* res = new instance::search::Continuous(this, scope, std::move(constraints));
            res->involvedChanged() += res * [res](bool v)
            {
                if(!v)
                {
                    delete res;
                }
            };

            return cmt::readyFuture(res->opposite());
        };

    //    in oneStage(
    //        query::Scope scope,
    //        pql::Expression constraints
    //    ) -> query::Result;
        _featureService->oneStage() += this * [this](
                                query::Scope scope,
                                pql::Expression&& constraints)
        {
            auto* res = new instance::search::OneStage(this, scope, std::move(constraints));
            res->involvedChanged() += res * [res](bool v)
            {
                if(!v)
                {
                    delete res;
                }
            };

            return cmt::readyFuture(res->opposite());
        };

    //    in record(
    //        link::Id,
    //        list<pql::Column> columns
    //    ) -> tuple<link::Remote, list<pql::Value>>;
        _featureService->record() += this * [this](
                                const link::Id& id,
                                const List<pql::Column>& columns)
        {
            instance::table::Record rec = _table.get(id);

            List<pql::Value> values;

            for(const pql::Column& c : columns)
            {
                values.emplace_back(rec.value(c));
            }

            link::Remote<> remote = rec.sv_remote().empty() ? link::Remote<>{} : *rec.sv_remote().begin();

            return cmt::readyFuture(Tuple{remote, values});
        };

    //    in select(
    //        query::Scope scope,
    //        pql::Expression constraints,
    //        list<pql::Column> columns,
    //        uint32 limit
    //    ) -> list<tuple<link::Id, link::Remote, list<pql::Value>>>;
        _featureService->select() += this * [this](
                                query::Scope scope,
                                const pql::Expression& constraints,
                                const List<pql::Column>& columns,
                                uint32 limit)
        {
            List<Tuple<link::Id, link::Remote<>, List<pql::Value>>> res;

            instance::table::Cursor tc = _table.enumerateRecords();

            while(tc && limit > res.size())
            {
                if(instance::search::checkOne(scope, constraints, tc))
                {
                    List<pql::Value> values;

                    for(const pql::Column& c : columns)
                    {
                        values.emplace_back(tc.value(c));
                    }

                    const Set<link::Remote<>>& remotes = tc.sv_remote();
                    link::Remote<> r = remotes.empty() ? link::Remote<>{} : *remotes.begin();
                    res.emplace_back(std::forward_as_tuple(tc.sv_id().data, std::move(r), std::move(values)));
                }
                tc.next();
            }

            return cmt::readyFuture(res);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Instance::~Instance()
    {
        flush();
        _featureService.reset();
        _table.reset();

        std::set<instance::Search*> searches(std::move(_searches));
        for(instance::Search* search : searches)
        {
            delete search;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<void> Instance::initialize(List<api::Feature<>>&& features)
    {
        std::vector<cmt::Future<List<pql::Column>>> columnSpecsByFeatures0;
        for(auto& feature : features)
        {
            columnSpecsByFeatures0.emplace_back(feature->setup(_featureService));
        }

        return cmt::whenAll(columnSpecsByFeatures0).chain() += this * [columnSpecsByFeatures0=std::move(columnSpecsByFeatures0), features = std::move(features), this]() mutable
        {
            dbgAssert(columnSpecsByFeatures0.size() == features.size());

            try
            {
                //initialize table

                std::vector<List<pql::Column>> columnSpecsByFeatures;
                columnSpecsByFeatures.reserve(columnSpecsByFeatures.size());
                for(auto& fut : columnSpecsByFeatures0)
                {
                    columnSpecsByFeatures.emplace_back(fut.detachValue());
                }

                _table.init(std::move(columnSpecsByFeatures));

                //subscribe

                for(std::size_t featureIdx(0); featureIdx<features.size(); ++featureIdx)
                {
                    auto& feature = features[featureIdx];

                    feature->updateRecord() += this * [this, featureIdx](const link::Id& id, List<pql::Value>&& values)
                    {
                        _table.updateRecord(featureIdx, id, std::move(values));
                    };
                }
            }
            catch(...)
            {
                flush();
                _table.reset();

                std::rethrow_exception(dci::exception::buildInstance<api::Error>(std::current_exception(), "rdb instance initialization failed"));
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::searchSubscribe(instance::Search* s)
    {
        dbgAssert(!_searches.count(s));
        _searches.insert(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::searchUnsubscribe(instance::Search* s)
    {
        dbgAssert(_searches.count(s));
        _searches.erase(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    instance::table::Cursor Instance::enumerateRecords()
    {
        return _table.enumerateRecords();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::online(const instance::table::Record& rec)
    {
        for(instance::Search* search : _searches)
        {
            search->online(rec);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::offline(const instance::table::Record& rec)
    {
        for(instance::Search* search : _searches)
        {
            search->offline(rec);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::inserted(const instance::table::Record& rec)
    {
        for(instance::Search* search : _searches)
        {
            search->inserted(rec);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::updated(const instance::table::Record& rec)
    {
        for(instance::Search* search : _searches)
        {
            search->updated(rec);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Instance::remove(const instance::table::Record& rec)
    {
        for(instance::Search* search : _searches)
        {
            search->remove(rec);
        }
    }
}
