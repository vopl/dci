/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "grid.hpp"
#include "../lis.hpp"

namespace dci::module::ppn::topology::lis
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Grid::Grid(Lis* lis)
        : _lis{lis}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Grid::~Grid()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::start()
    {
        reindex();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::stop()
    {
        _bars.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::setup(uint8 bits, uint16 size)
    {
        if(_kernel.change(bits, size))
        {
            if(_lis->started())
            {
                reindex();
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const grid::Kernel& Grid::kernel() const
    {
        return _kernel;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Grid::fillRequest(List<api::GridFillingStep>& filling)
    {
        std::size_t i{0};
        api::GridFillingStep step{0,0};
        enum class State
        {
            collectFull,
            collectEmpty
        } state {State::collectFull};

        while(i<_bars.size())
        {
            switch(state)
            {
            case State::collectFull:
                if(_bars[i]->onlineSize())
                {
                    step.full++;
                    if(step.full == 0xff)
                    {
                        dbgAssert(step.empty == 0);
                        filling.push_back(step);
                        step.full = 0;
                    }
                }
                else
                {
                    state = State::collectEmpty;
                    step.empty++;
                }
                break;

            case State::collectEmpty:
                if(!_bars[i]->onlineSize())
                {
                    step.empty++;
                    if(step.empty == 0xff)
                    {
                        filling.push_back(step);
                        step.full = 0;
                        step.empty = 0;
                    }
                }
                else
                {
                    filling.push_back(step);

                    state = State::collectFull;
                    step.full = 1;
                    step.empty = 0;
                }
                break;
            }

            i++;
        }

        if(step.empty > 0)
        {
            filling.push_back(step);
        }

        return filling.size() > 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::joined(const space::Id& id, const node::link::Remote<>& r)
    {
        space::Csid csid = _lis->csid(id);
        bar(csid).joined(csid, id, r);

        std::string s;
        std::size_t cc{};
        for(const grid::BarPtr& bar : _bars)
        {
            std::size_t c = bar->onlineSize();
            cc += c;
                 if(c < 1 ) s += '_';
            else if(c < 10) s += static_cast<char>('0'+c-0);
            else if(c < 36) s += static_cast<char>('a'+c-10);
            else if(c < 62) s += static_cast<char>('A'+c-36);
            else            s += '*';
        }

        LOGD(s<<" "<<cc);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::disjoined(const space::Id& id, const node::link::Remote<>& r)
    {
        space::Csid csid = _lis->csid(id);
        bar(csid).disjoined(csid, id, r);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::state(const space::Id& id, real64 rating)
    {
        space::Csid csid = _lis->csid(id);
        bar(csid).rating(csid, id, rating);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::demand(const space::Id& id, real64 weight, demand::NeedHolder<>&& d)
    {
        space::Csid csid = _lis->csid(id);
        bar(csid).demand(csid, id, weight, std::move(d));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    grid::Bar& Grid::bar(space::Csid csid)
    {
        dbgAssert(_kernel.size() == _bars.size());
        return *_bars[_kernel.idx(csid)];
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::requestRatings(space::Csid csidStart, space::Csid csidStop)
    {
        return _lis->requestState(csidStart, csidStop);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::requestDemand(const space::Id& id, real64 weight)
    {
        return _lis->requestDemand(id, weight);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Grid::reindex()
    {
        std::deque<grid::BarPtr> bars{std::exchange(_bars, {})};

        _bars.resize(_kernel.size());
        for(uint16 idx{0}; idx<_bars.size(); ++idx)
        {
            _bars[idx] = std::make_unique<grid::Bar>(
                             this,
                             _kernel.csid(idx+1),
                             _kernel.csid(idx));
        }

        dbgAssertX(bars.empty(), "move state from old to new bars is not implemented yet");

        for(std::size_t idx{0}; idx<_bars.size(); ++idx)
        {
            _bars[idx]->start();
        }
    }
}
