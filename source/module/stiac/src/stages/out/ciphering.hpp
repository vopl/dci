// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../base.hpp"
#include "../../crypto/symmetric.hpp"

namespace dci::module::stiac::stages::out
{
    class Ciphering
        : public Base
        , public crypto::Symmetric
    {
    public:
        using Base::Base;

        void urgent(MessageType mt, const void* data, uint32 dataSize);
        void allowPayload();

    private:
        uint16 getWantedEmptyPrefix() const override;
        void input(Bytes&& payload) override;
        Bytes flushOutput() override;

    private:
        void flushPayload();
        void encrypt(Bytes&& chunk, bool mixCiphertext2Hash);

    private:
        bool    _payloadAllowed = false;
        Bytes   _payload;
    };

    using CipheringPtr = std::unique_ptr<Ciphering>;
}
