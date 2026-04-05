/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "streamCipher.hpp"
#include <array>

namespace dci::crypto::impl
{
    class ChaCha final
        : public StreamCipher
    {
    public:
        ChaCha(std::size_t rounds);
        ChaCha(const ChaCha&);
        ChaCha(ChaCha&&);
        ~ChaCha() override;

        ChaCha& operator=(const ChaCha&);
        ChaCha& operator=(ChaCha&&);

    public:
        void setKey(const void* key, std::size_t len) override;
        void setIv(const void* iv, std::size_t len) override;
        void cipher(const void* in, void* out, std::size_t len) override;
        void seek(std::uint64_t offset) override;
        void clear() override;

    private:
        std::size_t                 _rounds;
        std::array<uint32_t, 8>     _key;
        std::size_t                 _keySize = 0;
        std::array<uint32_t, 16>    _state;
        std::array<uint8_t, 8*64>   _buffer;
        std::size_t                 _position = 0;

    };
}
