/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <cstdint>
#include "hash.hpp"
#include <array>

namespace dci::crypto::impl
{
    class Blake2s final
        : public Hash
    {
    public:
        Blake2s(std::size_t digestSize);
        Blake2s(const Blake2s&);
        Blake2s(Blake2s&&);
        ~Blake2s() override;

        Blake2s& operator=(const Blake2s&);
        Blake2s& operator=(Blake2s&&);

        HashPtr clone() override;

        std::size_t blockSize() override;
        void add(const void* data, std::size_t len) override;
        void barrier() override;
        void finish(void* digest) override;
        void finish(void* digest, std::size_t customDigestSize) override;
        void clear() override;

    private:
        void compress(const uint8_t* input, size_t blocks, uint64_t increment);

    public:
        static constexpr std::size_t BLOCKBYTES = 64;
        static constexpr std::size_t IVU32COUNT = 8;

    private:
        std::array<uint8_t, BLOCKBYTES>     _buffer;
        size_t                              _bufpos = 0;

        std::array<uint32_t, IVU32COUNT>    _H;
        std::array<uint32_t, 2>             _T;
        std::array<uint32_t, 2>             _F;
    };
}
