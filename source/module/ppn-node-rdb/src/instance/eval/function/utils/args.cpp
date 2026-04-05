/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "args.hpp"
#include "../../../eval.hpp"

namespace dci::module::ppn::node::rdb::instance::eval::function::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Args::Args(const List<pql::Expression>& raw, const table::Record& record)
        : _raw(raw)
        , _record(record)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const table::Record& Args::record() const
    {
        return _record;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Args::size() const
    {
        return _raw.size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const pql::Value& Args::operator[](std::size_t idx) const
    {
        if(idx >= size())
        {
            dbgWarn("bad index");
            return _nullStub;
        }

        while(_combined.size() <= idx)
        {
            _combined.emplace_back(combine(_raw[_combined.size()], _record).getOr(_nullStub));
        }

        return _combined[idx];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const List<pql::Value>& Args::head(std::size_t amount) const
    {
        dbgAssert(_raw.size() >= amount);

        while(_combined.size() < amount)
        {
            _combined.emplace_back(combine(_raw[_combined.size()], _record).getOr(_nullStub));
        }

        return _combined;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const pql::Value Args::_nullStub {};
}
