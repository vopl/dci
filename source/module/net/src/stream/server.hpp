// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include "pch.hpp"
#include "../optionsStore.hpp"

namespace dci::module::net
{
    class Host;

    namespace stream
    {
        class Server
            : public api::stream::Server<>::Opposite
            , public sbs::Owner
            , public mm::heap::Allocable<Server>
            , private OptionsStore
        {
        public:
            Server(Host* host);
            ~Server();

        private:
            cmt::Future<None> listen(auto&& endpoint);
            void close();
            void sockReady(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState);

        private:
            Host *              _host;
            api::Endpoint       _bindEndpoint;
            api::Endpoint       _localEndpoint;
            poll::Descriptor    _sock;
        };
    }
}
