// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../base.hpp"
#include "../../crypto/symmetric.hpp"

namespace dci::module::stiac::stages::in
{
    class Ciphering
        : public Base
        , public crypto::Symmetric
    {
    public:
        using Base::Base;

    private:
        void input(Bytes&& data) override;

    private:
        bool readType();
        bool readSize();
        bool readPayload();

    private:
        Bytes       _input;

        enum class State
        {
            awaitType,
            awaitSize,
            awaitPayload,
            bad,
        } _state {State::awaitType};

        MessageType _messageType = MessageType::fakeNull;
        uint32      _messageSize = 0;
        bool        _messageMix2Hash = false;

        Bytes       _payload;
    };

    using CipheringPtr = std::unique_ptr<Ciphering>;
}
