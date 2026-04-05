/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "mac.hpp"
#include <vector>

namespace dci::crypto::impl
{
    class Hmac final
        : public Mac
    {
    public:
        Hmac(HashPtr hash);
        Hmac(const Hmac&);
        Hmac(Hmac&&);
        ~Hmac() override;

        Hmac& operator=(const Hmac&);
        Hmac& operator=(Hmac&&);

        HashPtr clone() override;

        std::size_t blockSize() override;
        void setKey(const void* key, std::size_t len) override;
        void add(const void* data, std::size_t len) override;
        void barrier() override;
        void finish(void* digest) override;
        void finish(void* digest, std::size_t customDigestSize) override;
        void clear() override;

    private:
        HashPtr                 _hash;
        std::vector<uint8_t>    _ikey;
        std::vector<uint8_t>    _okey;
    };
}
