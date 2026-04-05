/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
