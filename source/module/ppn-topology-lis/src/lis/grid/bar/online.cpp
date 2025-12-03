// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "online.hpp"
#include "../bar.hpp"
#include "../../../lis.hpp"

namespace dci::module::ppn::topology::lis::grid::bar
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Online::Online()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Online::~Online()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Online::empty() const
    {
        return _records.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Online::size() const
    {
        std::size_t res {};

        for(const auto&[csid, record] : _records)
        {
            (void)csid;
            res += record._remotes.size();
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::joined(space::Csid csid, const space::Id& id, const node::link::Remote<>& r)
    {
        (void)id;

        Record& record = _records[csid];
        record._remotes.insert(Remote{id, r});
        updateDemand();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::disjoined(space::Csid csid, const space::Id& id, const node::link::Remote<>& r)
    {
        auto iter = _records.find(csid);
        if(_records.end() == iter)
        {
            return;
        }

        Record& record = iter->second;
        record._remotes.erase(Remote{id, r});
        if(record._remotes.empty())
        {
            bool needUpdateDemand = record._demand || record._demandRequested;
            _records.erase(iter);

            if(needUpdateDemand)
            {
                updateDemand();
            }
            return;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::demand(space::Csid csid, const space::Id& id, real64 weight, demand::NeedHolder<>&& d)
    {
        (void)id;
        (void)weight;

        auto iter = _records.find(csid);
        if(_records.end() == iter)
        {
            return;
        }

        Record& record = iter->second;
        if(!record._demandRequested)
        {
            return;
        }

        record._demandRequested = false;
        record._demand = std::move(d);
        updateDemand();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Online::updateDemand()
    {
        if(_records.empty())
        {
            return;
        }

        Bar* bar = static_cast<Bar *>(this);

        space::Csid center = static_cast<space::Csid>(bar->_csidBegin + (bar->_csidEnd - bar->_csidBegin)/2);

        Records::iterator fwd = _records.lower_bound(center);
        if(_records.end() == fwd) --fwd;

        dbgAssert(!fwd->second._remotes.empty());
        Remote remote = *fwd->second._remotes.begin();
        bool needDemand;
        {
            Record& record = fwd->second;
            needDemand = !record._demand && !record._demandRequested;
            if(needDemand)
            {
                record._demandRequested = true;
            }
        }

        Records::reverse_iterator bwd{fwd};
        bwd++;
        ++fwd;

        while(fwd != _records.end())
        {
            fwd->second._demandRequested = false;
            fwd->second._demand.reset();
            ++fwd;
        }

        while(bwd != _records.rend())
        {
            bwd->second._demandRequested = false;
            bwd->second._demand.reset();
            ++bwd;
        }

        if(needDemand)
        {
            bar->requestDemand(remote._id, _demandWeight);
        }
    }
}
