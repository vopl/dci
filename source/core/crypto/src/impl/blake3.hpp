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
#include "mac.hpp"

#define BLAKE3_KEY_LEN 32
#define BLAKE3_OUT_LEN 32
#define BLAKE3_BLOCK_LEN 64
#define BLAKE3_CHUNK_LEN 1024
#define BLAKE3_MAX_DEPTH 54

typedef struct {
    uint32_t cv[8];
    uint64_t chunk_counter;
    uint8_t buf[BLAKE3_BLOCK_LEN];
    uint8_t buf_len;
    uint8_t blocks_compressed;
    uint8_t flags;
} blake3_chunk_state;

typedef struct {
    uint32_t key[8];
    blake3_chunk_state chunk;
    uint8_t cv_stack_len;
    // The stack size is MAX_DEPTH + 1 because we do lazy merging. For example,
    // with 7 chunks, we have 3 entries in the stack. Adding an 8th chunk
    // requires a 4th entry, rather than merging everything down to 1, because we
    // don't know whether more input is coming. This is different from how the
    // reference implementation does things.
    uint8_t cv_stack[(BLAKE3_MAX_DEPTH + 1) * BLAKE3_OUT_LEN];
} blake3_hasher;


namespace dci::crypto::impl
{
    class Blake3 final
            : public Mac
    {
    public:
        Blake3(std::size_t digestSize);
        Blake3(std::size_t digestSize, std::array<std::uint8_t, 32> key);
        Blake3(std::size_t digestSize, const void* kdfMaterial, std::size_t kdfMaterialSize);
        Blake3(const Blake3&);
        Blake3(Blake3&&);
        ~Blake3() override;

        Blake3& operator=(const Blake3&);
        Blake3& operator=(Blake3&&);

        HashPtr clone() override;

        std::size_t blockSize() override;
        void add(const void* data, std::size_t len) override;
        void barrier() override;
        void finish(void* digest) override;
        void finish(void* digest, std::size_t customDigestSize) override;
        void clear() override;

    public:
        void setKey(const void* key, std::size_t len) override;
        void setKdfMaterial(const void* key, std::size_t len);

    private:
        blake3_hasher _hasher;
    };
}
