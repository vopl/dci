// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"
#include "dci/poll/descriptor/native.hpp"
#include "../optionsStore.hpp"
#include "../utils/recvBuffer.hpp"

namespace dci::module::net
{
    class Host;

    namespace datagram
    {
        class Channel
            : public api::datagram::Channel<>::Opposite
            , public sbs::Owner
            , public mm::heap::Allocable<Channel>
            , private OptionsStore
        {
        public:
            Channel(Host* host);
            ~Channel();

        private:
            void failed(ExceptionPtr e, bool doClose = false);
            void close();

            ExceptionPtr open(const api::Endpoint* bind = nullptr, const api::Endpoint* peer = nullptr);
            void doSend(poll::descriptor::Native native, const Bytes& data, const api::Endpoint& peer);
            void doRecv(poll::descriptor::Native native);
            void sockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState);

        private:
            Host *              _host;
            poll::Descriptor    _sock;
            api::Endpoint       _localEndpoint;

            bool                _opened = false;
            bool                _localEndpointFetched = false;
        };
    }
}
