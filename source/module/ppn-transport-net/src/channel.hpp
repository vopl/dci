// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::net
{
    class Channel
        : public sbs::Owner
        , public apit::Channel<>::Opposite
        , public mm::heap::Allocable<Channel>
    {
    public:
        Channel(apit::Address&& originalRemoteAddress, idl::gen::net::stream::Channel<>&& netStreamChannel);
        ~Channel();

    private:
        apit::Address                       _originalRemoteAddress;
        idl::gen::net::stream::Channel<>    _netStreamChannel;
    };
}
