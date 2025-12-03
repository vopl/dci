// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::inproc::channelBridge
{
    class Side
        : public sbs::Owner
    {
    public:
        Side();
        ~Side();

        void start(Side* parther, const apit::Address& address);
        void stop();

        void fail();
        void close();

        const apit::Channel<>::Opposite& ch() const;

    public:
        void pump();

    private:
        void wantPump();
        void unwantPump();

    private:
        void partnerGone();
        void input(Bytes&& data);

    private:
        bool                        _close {false};
        bool                        _fail {false};
        bool                        _closed {false};
        bool                        _inputLocked {true};
        Bytes                       _inputBuf;
        apit::Channel<>::Opposite   _ch{idl::interface::Initializer()};

        Side *                      _partner {};
    };
}
