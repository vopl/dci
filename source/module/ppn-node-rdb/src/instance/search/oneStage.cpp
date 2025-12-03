// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "oneStage.hpp"

namespace dci::module::ppn::node::rdb::instance::search
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    OneStage::OneStage(
            Instance* instance,
            query::Scope scope,
            pql::Expression&& constraints)
        : Search(instance, scope, std::move(constraints))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    OneStage::~OneStage()
    {
        stopImpl();
        flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool OneStage::startImpl()
    {
        if(!Search::startImpl())
        {
            return false;
        }

        table::Cursor tc = enumerateRecords();
        while(tc)
        {
            if(checkOne(tc))
            {
                emitResult(tc);
            }
            tc.next();
        }

        OneStage::stopImpl();

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool OneStage::stopImpl()
    {
        if(!Search::stopImpl())
        {
            return false;
        }

        return true;
    }
}
