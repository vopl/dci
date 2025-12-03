// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../channel.hpp"

namespace dci::module::www::tls::server
{
    class Channel
        : public api::tls::server::Channel<>::Opposite
        , public tls::Channel
        , public mm::heap::Allocable<Channel>
    {
    public:
        Channel(SSL_CTX* sslCtx, const Settings& settings, api::stream::Channel<>&& peer);
        ~Channel();

        cmt::Future<> handshake() override;
    private:
   };
}
