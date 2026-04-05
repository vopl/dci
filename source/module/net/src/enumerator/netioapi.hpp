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

namespace dci::module::net
{
    class Links;
    class Routes;

    namespace enumerator
    {
        class Netioapi
        {
        public:
            Netioapi(Links* links, Routes* routes);
            ~Netioapi();

        private:
            struct LinkChange;
            struct LinkChangeCommon;
            void onLinkChange(const LinkChange& change, std::shared_ptr<LinkChangeCommon>& cmn);

            struct RouteChange;
            void onRouteChange(const RouteChange& change);

            void flushChanges();

        private:
            Links *     _links {};
            Routes *    _routes {};

            HANDLE _hIpInterfaceChange{NULL};
            HANDLE _hRouteChange{NULL};

            std::mutex _mtx;
            struct LinkChange
            {
                NET_IFINDEX             _linkId;
                NET_LUID                _linkLuid;
                MIB_NOTIFICATION_TYPE   _ntype;
            };
            std::deque<LinkChange> _linkChanges;

            struct RouteChange
            {
                MIB_IPFORWARD_ROW2      _row;
                MIB_NOTIFICATION_TYPE   _ntype;
            };
            std::deque<RouteChange> _routeChanges;

            poll::Awaker _awaker{[this]{flushChanges();}, false};
        };
    }
}
