// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "mac.hpp"
#include <array>

namespace dci::crypto::impl
{
    class Poly1305 final
        : public Mac
    {
    public:
        Poly1305();
        Poly1305(const Poly1305&);
        Poly1305(Poly1305&&);
        ~Poly1305() override;

        Poly1305& operator=(const Poly1305&);
        Poly1305& operator=(Poly1305&&);

        HashPtr clone() override;

        std::size_t blockSize() override;
        void setKey(const void* key, std::size_t len) override;
        void add(const void* data, std::size_t len) override;
        void barrier() override;
        void finish(void* digest) override;
        void finish(void* digest, std::size_t customDigestSize) override;
        void clear() override;

    private:
        void blocks(const void* m, std::size_t blocks, bool is_final = false);

    private:
        std::array<uint64_t, 8> _poly;
        std::array<uint8_t, 16> _buf;
        size_t _bufPos = 0;
    };
}
