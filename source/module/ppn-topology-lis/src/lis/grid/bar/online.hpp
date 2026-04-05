/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "../../space.hpp"

namespace dci::module::ppn::topology::lis::grid::bar
{
    class Online
    {
    public:
        Online();
        ~Online();

        bool empty() const;
        std::size_t size() const;

        void joined     (space::Csid csid, const space::Id& id, const node::link::Remote<>& r);
        void disjoined  (space::Csid csid, const space::Id& id, const node::link::Remote<>& r);
        void demand     (space::Csid csid, const space::Id& id, real64 weight, demand::NeedHolder<>&& d);

    private:
        void updateDemand();

    private:
        struct Remote
        {
            space::Id               _id;
            node::link::Remote<>    _api;

            bool operator<(const Remote& other) const
            {
                return _api < other._api;
            }
        };

        struct Record
        {
            std::set<Remote>        _remotes;

            bool                    _demandRequested    {};
            demand::NeedHolder<>    _demand             {};
        };
        using Records = std::map<space::Csid, Record>;
        Records _records;

        static constexpr real64 _demandWeight {1.0};
    };
}
