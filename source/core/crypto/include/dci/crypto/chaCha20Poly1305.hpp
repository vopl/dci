// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO ChaCha20Poly1305
        : public himpl::FaceLayout<ChaCha20Poly1305, impl::ChaCha20Poly1305>
    {
    public:
        ChaCha20Poly1305();
        ChaCha20Poly1305(const ChaCha20Poly1305&);
        ChaCha20Poly1305(ChaCha20Poly1305&&);

        ChaCha20Poly1305& operator=(const ChaCha20Poly1305&);
        ChaCha20Poly1305& operator=(ChaCha20Poly1305&&);

        ~ChaCha20Poly1305();

    public:
        void setKey(const void* key, std::size_t len);
        void setAd(const void* ad, std::size_t len);

        void start(const void* nonce, std::size_t len);

        void encipher(const void* in, void* out, std::size_t len);
        void encipherFinish(void* macOut);

        void decipher(const void* in, void* out, std::size_t len);
        bool decipherFinish(const void* macIn);

        void clear();
    };
}
