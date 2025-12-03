// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
