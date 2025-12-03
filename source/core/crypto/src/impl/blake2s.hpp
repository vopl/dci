// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
