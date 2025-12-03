// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "mac.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO Hmac
        : public himpl::FaceLayout<Hmac, impl::Hmac, Mac>
    {
    public:
        static HashPtr alloc(HashPtr hash);

    public:
        Hmac(HashPtr hash);
        Hmac(const Hmac&);
        Hmac(Hmac&&);
        ~Hmac();

        Hmac& operator=(const Hmac&);
        Hmac& operator=(Hmac&&);

        HashPtr clone();

        void setKey(const void* key, std::size_t len);

        std::size_t blockSize();
        std::size_t digestSize();
        using Mac::add;
        void add(const void* data, std::size_t len);
        void barrier();
        void finish(void* digest);
        void finish(void* digest, std::size_t customDigestSize);
        void clear();
    };
}
