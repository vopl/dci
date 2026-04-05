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
#include "links.hpp"
#include "routes.hpp"

#ifdef _WIN32
#   include "enumerator/netioapi.hpp"
#else
#   include "enumerator/rtnetlink.hpp"
#endif
#include "ipResolver.hpp"

#include "stream/server.hpp"
#include "stream/client.hpp"
#include "stream/channel.hpp"
#include "datagram/channel.hpp"

#include "utils/recvBuffer.hpp"
#include "datagram/sendBuffer.hpp"

namespace dci::module::net
{
    class Host
        : public api::Host<>::Opposite
        , public sbs::Owner
        , public host::module::ServiceBase<Host>
    {
    public:
        Host();
        ~Host();

        void track(stream::Server* v);
        void untrack(stream::Server* v);

        void track(stream::Client* v);
        void untrack(stream::Client* v);

        void track(stream::Channel* v);
        void untrack(stream::Channel* v);

        void track(datagram::Channel* v);
        void untrack(datagram::Channel* v);

        utils::RecvBuffer* getRecvBuffer();
        datagram::SendBuffer* getDatagramSendBuffer();

    private:
        Links                   _links;
        Routes                  _routes;
#ifdef _WIN32
        enumerator::Netioapi    _enumerator{&_links, &_routes};
#else
        enumerator::Rtnetlink   _enumerator{&_links, &_routes};
#endif
        IpResolver              _ipResolver;

        std::set<stream::Server*>       _streamServers;
        std::set<stream::Client*>       _streamClients;
        std::set<stream::Channel*>      _streamChannels;
        std::set<datagram::Channel*>    _datagramChannels;

        utils::RecvBuffer       _recvBuffer;
        datagram::SendBuffer    _datagramSendBuffer;

    };
}
