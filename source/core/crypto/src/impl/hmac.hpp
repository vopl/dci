// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "mac.hpp"
#include <vector>

namespace dci::crypto::impl
{
    class Hmac final
        : public Mac
    {
    public:
        Hmac(HashPtr hash);
        Hmac(const Hmac&);
        Hmac(Hmac&&);
        ~Hmac() override;

        Hmac& operator=(const Hmac&);
        Hmac& operator=(Hmac&&);

        HashPtr clone() override;

        std::size_t blockSize() override;
        void setKey(const void* key, std::size_t len) override;
        void add(const void* data, std::size_t len) override;
        void barrier() override;
        void finish(void* digest) override;
        void finish(void* digest, std::size_t customDigestSize) override;
        void clear() override;

    private:
        HashPtr                 _hash;
        std::vector<uint8_t>    _ikey;
        std::vector<uint8_t>    _okey;
    };
}
