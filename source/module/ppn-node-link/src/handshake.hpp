// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "remote/state.hpp"

namespace dci::module::ppn::node::link
{
    class Local;

    class Handshake;
    using HandshakePtr = std::unique_ptr<Handshake>;

    class Handshake
        : public mm::heap::Allocable<Handshake>
        , public sbs::Owner
    {
    public:
        Handshake(Local* local, apit::Channel<>&& channel, bool byConnect);
        ~Handshake();


        cmt::Future<api::Remote<>> completion();

        void start();

    private:
        void setupProtocol();

    private:
        Local *                     _local;
        remote::State               _state;
        cmt::Promise<api::Remote<>> _completion;
    };
}
