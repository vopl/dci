// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include "hash.hpp"
#include <array>

namespace dci::crypto::impl
{
    class Sha2_512 final
        : public Hash
    {
    public:
        Sha2_512(std::size_t digestSize);
        Sha2_512(const Sha2_512&);
        Sha2_512(Sha2_512&&);
        ~Sha2_512() override;

        Sha2_512& operator=(const Sha2_512&);
        Sha2_512& operator=(Sha2_512&&);

        HashPtr clone() override;

        std::size_t blockSize() override;
        void add(const void* data, std::size_t len) override;
        void barrier() override;
        void finish(void* digest) override;
        void finish(void* digest, std::size_t customDigestSize) override;
        void clear() override;

    private:
        void transform(const void* data);
        void transform(const std::uint64_t* data);

        std::array<std::uint64_t, 8>    _state;
        std::uint64_t                   _bitcount;
        std::array<std::uint8_t, 128>   _buffer;
    };
}
