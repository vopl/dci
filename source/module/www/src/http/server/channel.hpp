// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "io/plexus.hpp"
#include "response.hpp"
#include "request.hpp"

namespace dci::module::www::http::server
{
    class Channel
        : public api::http::server::Channel<>::Opposite
        , public mm::heap::Allocable<Channel>
        , public io::Plexus<Request, Response, true>
    {
    public:
        Channel(api::stream::Channel<>&& streamChannel);
        ~Channel();

    public:
        void emitIo(api::http::server::Request<>&& request);
    };
}
