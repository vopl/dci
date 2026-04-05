/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
