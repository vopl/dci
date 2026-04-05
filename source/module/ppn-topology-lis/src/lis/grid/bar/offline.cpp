/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "offline.hpp"
#include "../bar.hpp"

namespace dci::module::ppn::topology::lis::grid::bar
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Offline::Offline()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Offline::~Offline()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Offline::start()
    {
        if(!_started)
        {
            _started = true;
            Bar* bar = static_cast<Bar*>(this);
            bar->requestRatings();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Offline::stop()
    {
        if(_started)
        {
            _started = false;
            _records.clear();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Offline::rating(space::Csid csid, const space::Id& id, real64 v)
    {
        Bar* bar = static_cast<Bar*>(this);

        real64 pos = csid;
        real64 center = (static_cast<real64>(bar->_csidBegin) + static_cast<real64>(bar->_csidEnd))/2;
        real64 range = (static_cast<real64>(bar->_csidEnd) - static_cast<real64>(bar->_csidBegin))/2;

        Record& record = _records[id];

        record._rating = std::max(v, 0.0);
        real64 demandWeight4Centering = std::clamp(fabs(pos - center) / range * 2, 0.25, 1.0);
        real64 demandWeight4Pos = (2.0 - center/0x1p64);
        real64 demandWeight =
                std::pow(record._rating, 0.25) *
                demandWeight4Centering *
                demandWeight4Pos;

        if(demandWeight == record._demandWeight)
        {
            return;
        }

        if(demandWeight <= 0)
        {
            record._demandWeight = 0;
            record._demand.reset();
            return;
        }

        dbgAssert(demandWeight);
        record._demandWeight = demandWeight;
        if(record._demand)
        {
            record._demand->reweight(record._demandWeight);
            return;
        }

        if(record._demandRequested)
        {
            return;
        }

        record._demandRequested = true;
        bar->requestDemand(id, record._demandWeight);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Offline::demand(space::Csid csid, const space::Id& id, real64 weight, demand::NeedHolder<>&& d)
    {
        (void)csid;

        auto iter = _records.find(id);
        if(_records.end() == iter)
        {
            return;
        }

        Record& record = iter->second;
        record._demandRequested = false;

        if(record._demandWeight <= 0)
        {
            record._demand.reset();
            return;
        }

        record._demand = std::move(d);
        if(!record._demand)
        {
            return;
        }

        if(weight != record._demandWeight)
        {
            record._demand->reweight(record._demandWeight);
        }
    }
}
