// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
