// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "hash.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO Mac
        : public himpl::FaceLayout<Mac, impl::Mac, Hash>
    {
    protected:
        Mac(const Mac&) = delete;
        Mac(Mac&&) = delete;

        Mac& operator=(const Mac&) = delete;
        Mac& operator=(Mac&&) = delete;

    public:
        Mac(himpl::FakeConstructionArg fc);
        ~Mac();

    public:
        void setKey(const void* key, std::size_t len);
    };
}
