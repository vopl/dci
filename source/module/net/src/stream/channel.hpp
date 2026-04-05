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
#include "dci/poll/descriptor/native.hpp"
#include "pch.hpp"
#include "../optionsStore.hpp"
#include "../utils/recvBuffer.hpp"
#include "sendBuffer.hpp"

namespace dci::module::net
{
    class Host;

    namespace stream
    {
        class Channel
            : public api::stream::Channel<>::Opposite
            , public sbs::Owner
            , public mm::heap::Allocable<Channel>
            , public OptionsStore
        {
        public:
            Channel(
                    Host* host,
                    poll::descriptor::Native sock,
                    const api::Endpoint& localEndpoint,
                    api::Endpoint&& remoteEndpoint);

            ~Channel();

        public:
            cmt::Future<api::stream::Channel<>> connect(bool needBind);

        private:
            void setReceiveGranula(uint64 granula);

            void failed(ExceptionPtr e, bool doClose = false);
            void shutdown(bool input, bool output);
            void close();

            bool doWrite(poll::descriptor::Native native, bool preCloseMode = false);
            bool doRead(poll::descriptor::Native native);

            void connectSockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState);
            void connectedSockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState);

        private:
            Host *              _host;
            poll::Descriptor    _sock;
            sbs::Owner          _sockReadyOwner;
            api::Endpoint       _localEndpoint;
            api::Endpoint       _remoteEndpoint;

            SendBuffer                              _sendBuffer;
            poll::descriptor::ReadyStateFlags       _lastReadyState{};
            cmt::Promise<api::stream::Channel<>>    _connectPromise;

            bool                _connected = false;
            uint32              _receiveGranula = 0;

            using AliveMarker = std::shared_ptr<bool>;
            AliveMarker _aliveMarker{std::make_shared<bool>(true)};
        };
    }
}
