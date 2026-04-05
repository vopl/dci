/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
