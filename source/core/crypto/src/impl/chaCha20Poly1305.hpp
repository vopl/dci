/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "chaCha.hpp"
#include "poly1305.hpp"
#include <cstdint>
#include <vector>

namespace dci::crypto::impl
{
    class ChaCha20Poly1305 final
    {
    public:
        ChaCha20Poly1305();
        ChaCha20Poly1305(const ChaCha20Poly1305&);
        ChaCha20Poly1305(ChaCha20Poly1305&&);
        ~ChaCha20Poly1305();

        ChaCha20Poly1305& operator=(const ChaCha20Poly1305&);
        ChaCha20Poly1305& operator=(ChaCha20Poly1305&&);

    public:
        void setKey(const void* key, std::size_t len);
        void setAd(const void* ad, std::size_t len);

        void start(const void* nonce, std::size_t len);

        void encipher(const void* in, void* out, std::size_t len);
        void encipherFinish(void* macOut);

        void decipher(const void* in, void* out, std::size_t len);
        bool decipherFinish(const void* macIn);

        void clear();

    private:
        bool cfrgVersion() const;
        void updateLen(std::size_t);

    private:
        ChaCha                      _chaCha;
        Poly1305                    _poly1305;
        std::vector<std::uint8_t>   _ad;
        std::size_t                 _nonceLen = 0;
        std::size_t                 _ctextLen = 0;
    };
}
