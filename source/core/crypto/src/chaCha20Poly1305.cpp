/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/crypto/chaCha20Poly1305.hpp>
#include "impl/chaCha20Poly1305.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::ChaCha20Poly1305()
        : himpl::FaceLayout<ChaCha20Poly1305, impl::ChaCha20Poly1305>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::ChaCha20Poly1305(const ChaCha20Poly1305& from)
        : himpl::FaceLayout<ChaCha20Poly1305, impl::ChaCha20Poly1305>(from.impl())
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::ChaCha20Poly1305(ChaCha20Poly1305&& from)
        : himpl::FaceLayout<ChaCha20Poly1305, impl::ChaCha20Poly1305>(std::move(from.impl()))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305& ChaCha20Poly1305::operator=(const ChaCha20Poly1305& from)
    {
        impl() = from.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305& ChaCha20Poly1305::operator=(ChaCha20Poly1305&& from)
    {
        impl() = std::move(from.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::~ChaCha20Poly1305()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::setKey(const void* key, std::size_t len)
    {
        return impl().setKey(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::setAd(const void* ad, std::size_t len)
    {
        return impl().setAd(ad, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::start(const void* nonce, std::size_t len)
    {
        return impl().start(nonce, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::encipher(const void* in, void* out, std::size_t len)
    {
        return impl().encipher(in, out, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::encipherFinish(void* macOut)
    {
        return impl().encipherFinish(macOut);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::decipher(const void* in, void* out, std::size_t len)
    {
        return impl().decipher(in, out, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool ChaCha20Poly1305::decipherFinish(const void* macIn)
    {
        return impl().decipherFinish(macIn);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::clear()
    {
        return impl().clear();
    }
}
