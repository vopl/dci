// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "hash.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO Blake2b
        : public himpl::FaceLayout<Blake2b, impl::Blake2b, Hash>
    {
    public:
        static HashPtr alloc(std::size_t digestSize = 64);

    public:
        Blake2b(std::size_t digestSize = 64);
        Blake2b(const Blake2b&);
        Blake2b(Blake2b&&);
        ~Blake2b();

        Blake2b& operator=(const Blake2b&);
        Blake2b& operator=(Blake2b&&);

        HashPtr clone();

        std::size_t blockSize();
        std::size_t digestSize();
        using Hash::add;
        void add(const void* data, std::size_t len);
        void barrier();
        void finish(void* digest);
        void finish(void* digest, std::size_t customDigestSize);
        void clear();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void API_DCI_CRYPTO blake2b(const void* data, std::size_t len, void* digest, std::size_t digestSize = 64);
}
