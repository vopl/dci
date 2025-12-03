// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "mac.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO Poly1305
        : public himpl::FaceLayout<Poly1305, impl::Poly1305, Mac>
    {
    public:
        static HashPtr alloc();

    public:
        Poly1305();
        Poly1305(const Poly1305&);
        Poly1305(Poly1305&&);
        ~Poly1305();

        Poly1305& operator=(const Poly1305&);
        Poly1305& operator=(Poly1305&&);

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
