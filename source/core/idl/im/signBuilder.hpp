// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "sign.hpp"
#include <dci/crypto/blake2b.hpp>

#include <string>
#include <string_view>
#include <cstdint>

namespace dci::idl::im
{
    //Sign как 128 бит половинка от Blake2b-256
    class SignBuilder final
    {
    public:
        SignBuilder();
        SignBuilder(const SignBuilder& other);
        ~SignBuilder();

        SignBuilder& operator=(const SignBuilder& other);

        void add(const Sign& v);

        using Full = std::array<std::uint8_t, 32>;
        void add(const Full& v);

        void add(const std::string& v);
        void add(std::string_view v);
        void add(const char* csz);

        void add(bool v);

        void add(std::uint8_t v);
        void add(std::uint16_t v);
        void add(std::uint32_t v);
        void add(std::uint64_t v);

        void add(std::int8_t v);
        void add(std::int16_t v);
        void add(std::int32_t v);
        void add(std::int64_t v);

        Full finish();

    private:
        void addImpl(std::string_view tag, const void* data, std::size_t size);

    private:
        crypto::Blake2b _hashier{Full{}.size()};
    };
}
