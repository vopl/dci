// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include "hash.hpp"

namespace dci::crypto::impl
{
    class Mac
        : public Hash
    {
    public:
        Mac(std::size_t digestSize);
        Mac(const Mac&);
        Mac(Mac&&);
        ~Mac() override;

        Mac& operator=(const Mac&);
        Mac& operator=(Mac&&);

        virtual void setKey(const void* key, std::size_t len);
    };
}
