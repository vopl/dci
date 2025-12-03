// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "io/plexus.hpp"
#include "response.hpp"
#include "request.hpp"

namespace dci::module::www::http::client
{
    class Channel
        : public api::http::client::Channel<>::Opposite
        , public mm::heap::Allocable<Channel>
        , public io::Plexus<Response, Request, false>
    {
    public:
        Channel(api::stream::Channel<>&& streamChannel);
        ~Channel();
    };
}
