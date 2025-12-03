// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "mac.hpp"
#include <array>

namespace dci::crypto
{
    class API_DCI_CRYPTO Blake3
        : public himpl::FaceLayout<Blake3, impl::Blake3, Mac>
    {
    public:
        static HashPtr alloc(std::size_t digestSize = 32);

    public:
        Blake3(std::size_t digestSize = 32);// hash mode
        Blake3(std::size_t digestSize, std::array<std::uint8_t, 32> key);//mac mode
        Blake3(std::size_t digestSize, const void* kdfMaterial, std::size_t kdfMaterialSize);//kdf mode
        Blake3(const Blake3&);
        Blake3(Blake3&&);
        ~Blake3();

        Blake3& operator=(const Blake3&);
        Blake3& operator=(Blake3&&);

        HashPtr clone();

        std::size_t blockSize();
        std::size_t digestSize();
        using Hash::add;
        void add(const void* data, std::size_t len);
        void barrier();
        void finish(void* digest);
        void finish(void* digest, std::size_t customDigestSize);
        void clear();//reset to initial hash mode

    public:
        void setKey(const void* key, std::size_t len);//reset to initial mac mode
        void setKdfMaterial(const void* key, std::size_t len);//reset to initial kdf mode
        void setKdfMaterial(const char* keyz);//reset to initial kdf mode
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void API_DCI_CRYPTO blake3(const void* data, std::size_t len, void* digest, std::size_t digestSize = 32);
}
