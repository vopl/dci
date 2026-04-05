/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/himpl.hpp>
#include <dci/crypto/implMetaInfo.hpp>
#include "api.hpp"
#include "mac.hpp"

namespace dci::crypto
{
    class API_DCI_CRYPTO Hmac
        : public himpl::FaceLayout<Hmac, impl::Hmac, Mac>
    {
    public:
        static HashPtr alloc(HashPtr hash);

    public:
        Hmac(HashPtr hash);
        Hmac(const Hmac&);
        Hmac(Hmac&&);
        ~Hmac();

        Hmac& operator=(const Hmac&);
        Hmac& operator=(Hmac&&);

        HashPtr clone();

        void setKey(const void* key, std::size_t len);

        std::size_t blockSize();
        std::size_t digestSize();
        using Mac::add;
        void add(const void* data, std::size_t len);
        void barrier();
        void finish(void* digest);
        void finish(void* digest, std::size_t customDigestSize);
        void clear();
    };
}
