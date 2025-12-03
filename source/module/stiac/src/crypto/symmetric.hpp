// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "handshake.hpp"
#include "secret.hpp"

namespace dci::module::stiac::crypto
{
    class Symmetric
    {
    protected:
        static constexpr uint16 _keySize            = Handshake::_symmetricKeySize;
        static constexpr uint16 _hashSize           = 64;
        static constexpr uint16 _nonceSize          = 8;
        static constexpr uint16 _genericHeaderSize  = 1;
        static constexpr uint16 _payloadHeaderSize  = 2;
        static constexpr uint16 _macSize            = 16;

    protected:
        using MessageType = Handshake::MessageType;

    public:
        ~Symmetric();

        void setHandshake(Handshake* hs);

        void resetState();

        void mixHash(const Bytes& material);
        void mixHash(const char* cszMaterial);
        void mixHash(const void* materialData, uint32 materialSize);

        void mixKey(const uint8 material[_keySize]);

    protected:
        void messageStart();
        void mixHashStart();

        void messageEncipher(void* data, uint32 size);
        void messageDecipher(void* data, uint32 size);
        void mixHashUpdate(const void* data, uint32 size);

        void messageEncipherFinish(void* macOut);
        bool messageDecipherFinish(const void* macIn);
        void mixHashFinish();

    protected:
        Handshake *                         _hs = nullptr;
        bool                                _keySetted = false;
        dci::crypto::ChaCha20Poly1305       _aead;

        Secret<_hashSize>                   _hash;
        Secret<_hashSize>                   _chainingKey;

        dci::crypto::Blake2b                _hashMixer {_hashSize};
        dci::crypto::Hmac                   _mac4kdf {dci::crypto::Blake2b::alloc(64)};

        //как счетчик, обнуляется при смене ключа
        union Nonce
        {
            uint64  _counter {};
            uint8   _raw[_nonceSize];
        } _nonce {};
        static_assert(_nonceSize == sizeof(_nonce));
    };
}
