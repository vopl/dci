// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "hash.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO Sha2_512
        : public himpl::FaceLayout<Sha2_512, impl::Sha2_512, Hash>
    {
    public:
        static HashPtr alloc(std::size_t digestSize = 64);

    public:
        Sha2_512(std::size_t digestSize = 64);
        Sha2_512(const Sha2_512&);
        Sha2_512(Sha2_512&&);
        ~Sha2_512();

        Sha2_512& operator=(const Sha2_512&);
        Sha2_512& operator=(Sha2_512&&);

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
    void API_DCI_CRYPTO sha2_512(const void* data, std::size_t len, void* digest, std::size_t digestSize = 64);
}
