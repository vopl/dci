// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include <cstdint>

namespace dci::crypto
{
    class API_DCI_CRYPTO StreamCipher
        : public himpl::FaceLayout<StreamCipher, impl::StreamCipher>
    {
    protected:
        StreamCipher(const StreamCipher&) = delete;
        StreamCipher(StreamCipher&&) = delete;

        StreamCipher& operator=(const StreamCipher&) = delete;
        StreamCipher& operator=(StreamCipher&&) = delete;

    public:
        StreamCipher(himpl::FakeConstructionArg fc);
        ~StreamCipher();

    public:
        void setKey(const void* key, std::size_t len);
        void setIv(const void* iv, std::size_t len);
        void cipher(const void* in, void* out, std::size_t len);
        void seek(std::uint64_t offset);
        void clear();
    };
}
