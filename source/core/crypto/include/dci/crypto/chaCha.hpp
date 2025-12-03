// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "streamCipher.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO ChaCha
        : public himpl::FaceLayout<ChaCha, impl::ChaCha, StreamCipher>
    {
    public:
        ChaCha(std::size_t rounds=20);
        ChaCha(const ChaCha&);
        ChaCha(ChaCha&&);

        ChaCha& operator=(const ChaCha&);
        ChaCha& operator=(ChaCha&&);

        ~ChaCha();

    public:
        void setKey(const void* key, std::size_t len);
        void setIv(const void* iv, std::size_t len);
        void cipher(const void* in, void* out, std::size_t len);
        void seek(std::uint64_t offset);
        void clear();
    };
}
