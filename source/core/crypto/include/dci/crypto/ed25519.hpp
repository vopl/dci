// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <cstdint>

namespace dci::crypto::ed25519
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void API_DCI_CRYPTO mkPublic(
            const void* sk,
            void* pk);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void API_DCI_CRYPTO sign(
            const void* message, std::uint32_t messageLen,
            const void* pk, const void* sk,
            void* signature);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool API_DCI_CRYPTO verify(
            const void* message, std::uint32_t messageLen,
            const void* pk,
            const void* signature);
}
