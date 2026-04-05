/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "storage.hpp"

namespace dci::module::ppn::service::dht
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const api::Key& key, const api::Value& value)
    {
        std::chrono::steady_clock::time_point moment = std::chrono::steady_clock::now();

        auto& idx = _container.get<ByKeyValue>();
        auto iter = idx.lower_bound(std::tie(key, value));
        if(idx.end() != iter && iter->_value == value)
        {
            idx.modify(iter, [&](ValueHolder& vh)
            {
                vh._moment = moment;
            });
            return;
        }

        idx.emplace(key, value, moment);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::get(const api::Key& key, uint32 limit, Set<api::Value>& value)
    {
        auto& idx = _container.get<ByKey>();
        auto range = idx.equal_range(key);

        while(range.first != range.second && value.size() < limit)
        {
            value.insert(range.first->_value);
            ++range.first;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::dropOld()
    {
        std::chrono::steady_clock::time_point bound = std::chrono::steady_clock::now()-std::chrono::hours{12};
        auto& idx = _container.get<ByMoment>();
        idx.erase(idx.begin(), idx.upper_bound(bound));
    }
}
