/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "statI.hpp"
#include "../reest.hpp"

namespace dci::module::ppn::connectivity::reest
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StatI::StatI(Reest* srv)
        : _srv{srv}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StatI::~StatI()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatI::statChanged(StatIA* stat)
    {
        _stats.insert(stat);
        _srv->wantOnceFlush(this);
    }

    void StatI::statDead(StatIA* stat)
    {
        _stats.erase(stat);
        _top.erase(std::remove(_top.begin(), _top.end(), stat), _top.end());
        _topChanged = true;
        _srv->wantOnceFlush(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StatI::onceFlush()
    {
        static const std::size_t N = 10;
        std::vector<StatIA*> top;
        top.reserve(N);

        auto iter = _stats.begin();

        while(_stats.end() != iter && top.size() < N)
        {
            StatIA* sia = *iter;

            if(sia->rating() > 0)
            {
                top.push_back(sia);
            }

            ++iter;
        }

        std::sort(top.begin(), top.end(), [](const StatIA* a, const StatIA* b)
        {
            return std::tuple(a->rating(), a) >
                   std::tuple(b->rating(), b);
        });

//        for(auto v : top)
//        {
//            LOGD("rating: "<<v->rating()<<" for "<<std::get<1>(stat2Key(*v)).value);
//        }

        real64 rating0 = _top.empty() ? 0 : _top.front()->rating();
        if(_top != top || _rating0 != rating0 || _topChanged)
        {
            _topChanged = false;
            _top = std::move(top);
            _rating0 = rating0;

            _srv->statChanged(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::vector<StatIA*>& StatI::top() const
    {
        return _top;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool StatI::dead() const
    {
        return _stats.empty();
    }

}
