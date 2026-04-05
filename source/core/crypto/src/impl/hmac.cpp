/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "hmac.hpp"
#include <dci/crypto/hmac.hpp>
#include <dci/crypto/hash.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hmac::Hmac(HashPtr hash)
        : Mac{hash->digestSize()}
        , _hash{std::move(hash)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hmac::Hmac(const Hmac& from)
        : Mac{from}
        , _hash{from._hash->clone()}
        , _ikey(from._ikey)
        , _okey(from._okey)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hmac::Hmac(Hmac&& from)
        : Mac{std::move(from)}
        , _hash{std::move(from._hash)}
        , _ikey(std::move(from._ikey))
        , _okey(std::move(from._okey))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hmac::~Hmac()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hmac& Hmac::operator=(const Hmac& from)
    {
        Mac::operator=(from);
        _hash = from._hash->clone();
        _ikey = from._ikey;
        _okey = from._okey;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hmac& Hmac::operator=(Hmac&& from)
    {
        Mac::operator=(std::move(from));
        _hash = std::move(from._hash);
        _ikey = std::move(from._ikey);
        _okey = std::move(from._okey);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Hmac::clone()
    {
        return HashPtr
        {
            new crypto::Hmac{himpl::impl2Face<crypto::Hmac>(*this)},
            [](crypto::Hash* p){delete static_cast<crypto::Hmac*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Hmac::blockSize()
    {
        return _hash->blockSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hmac::setKey(const void* key, std::size_t len)
    {
        const std::uint8_t ipad = 0x36;
        const std::uint8_t opad = 0x5C;

        _hash->clear();

        std::size_t blockSize = _hash->blockSize();
        dbgAssert(blockSize >= _digestSize);

        _ikey.resize(blockSize);
        memset(_ikey.data(), ipad, blockSize);

        _okey.resize(blockSize);
        memset(_okey.data(), opad, blockSize);

        if(len > blockSize)
        {
            _hash->add(key, len);
            _hash->finish(_ikey.data());

            for(std::size_t i(0); i<_digestSize; ++i)
            {
                _okey[i] ^= _ikey[i];
                _ikey[i] ^= ipad;
            }
        }
        else
        {
            const uint8_t* key1 = static_cast<const uint8_t*>(key);
            for(std::size_t i(0); i<len; ++i)
            {
                _ikey[i] ^= key1[i];
                _okey[i] ^= key1[i];
            }
        }

        _hash->add(_ikey);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hmac::add(const void* data, std::size_t len)
    {
        _hash->add(data, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hmac::barrier()
    {
        _hash->barrier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hmac::finish(void* digest)
    {
        finish(digest, _hash->digestSize());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hmac::finish(void* digest, std::size_t customDigestSize)
    {
        _hash->finish(digest, customDigestSize);
        _hash->add(_okey);
        _hash->add(digest, _hash->digestSize());
        _hash->finish(digest);
        _hash->add(_ikey);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hmac::clear()
    {
        _hash->clear();
    }
}
