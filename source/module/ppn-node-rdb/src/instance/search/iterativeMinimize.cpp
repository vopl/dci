/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
