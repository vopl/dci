// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include "hash.hpp"
#include <array>

namespace dci::crypto::impl
{
    class Blake2b final
        : public Hash
    {
    public:
        Blake2b(std::size_t digestSize);
        Blake2b(const Blake2b&);
        Blake2b(Blake2b&&);
        ~Blake2b() override;

        Blake2b& operator=(const Blake2b&);
        Blake2b& operator=(Blake2b&&);

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
        static constexpr std::size_t BLOCKBYTES = 128;
        static constexpr std::size_t IVU64COUNT = 8;

    private:
        std::array<uint8_t, BLOCKBYTES>     _buffer;
        size_t                              _bufpos = 0;

        std::array<uint64_t, IVU64COUNT>    _H;
        std::array<uint64_t, 2>             _T;
        std::array<uint64_t, 2>             _F;
    };
}
