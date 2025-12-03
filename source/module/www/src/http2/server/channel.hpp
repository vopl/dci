// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::http2::server
{
    class Channel
        : public api::http2::server::Channel<>::Opposite
        , public host::module::ServiceBase<Channel>
    {
    public:
        Channel(api::stream::Channel<>&& streamChannel);
        ~Channel();

    private:
        api::stream::Channel<> _streamChannel;
    };
}
