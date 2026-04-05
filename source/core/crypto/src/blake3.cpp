/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/crypto/blake3.hpp>
#include "impl/blake3.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Blake3::alloc(std::size_t digestSize)
    {
        return HashPtr
        {
            new Blake3{digestSize},
            [](Hash*p){delete static_cast<Blake3*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(std::size_t digestSize)
        : himpl::FaceLayout<Blake3, impl::Blake3, Mac>{digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(std::size_t digestSize, std::array<std::uint8_t, 32> key)
        : himpl::FaceLayout<Blake3, impl::Blake3, Mac>{digestSize, key}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(std::size_t digestSize, const void* kdfMaterial, std::size_t kdfMaterialSize)
        : himpl::FaceLayout<Blake3, impl::Blake3, Mac>{digestSize, kdfMaterial, kdfMaterialSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(const Blake3& from)
        : himpl::FaceLayout<Blake3, impl::Blake3, Mac>{from.impl()}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(Blake3&& from)
        : himpl::FaceLayout<Blake3, impl::Blake3, Mac>{std::move(from.impl())}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::~Blake3()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3& Blake3::operator=(const Blake3& from)
    {
        impl() = from.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3& Blake3::operator=(Blake3&& from)
    {
        impl() = std::move(from.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Blake3::clone()
    {
        return impl().clone();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake3::blockSize()
    {
        return impl().blockSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake3::digestSize()
    {
        return impl().digestSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::add(const void* data, std::size_t len)
    {
        return impl().add(data, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::barrier()
    {
        return impl().barrier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::finish(void* digest)
    {
        return impl().finish(digest);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::finish(void* digest, std::size_t customDigestSize)
    {
        return impl().finish(digest, customDigestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::clear()
    {
        return impl().clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::setKey(const void* key, std::size_t len)
    {
        return impl().setKey(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::setKdfMaterial(const void* key, std::size_t len)
    {
        return impl().setKdfMaterial(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::setKdfMaterial(const char* keyz)
    {
        return impl().setKdfMaterial(keyz, std::strlen(keyz));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void blake3(const void* data, std::size_t len, void* digest, std::size_t digestSize)
    {
        impl::Blake3 impl{digestSize};
        impl.add(data, len);
        impl.finish(digest);
    }
}
