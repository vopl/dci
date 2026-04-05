/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/crypto/blake2s.hpp>
#include "impl/blake2s.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Blake2s::alloc(std::size_t digestSize)
    {
        return HashPtr
        {
            new Blake2s{digestSize},
            [](Hash*p){delete static_cast<Blake2s*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::Blake2s(std::size_t digestSize)
        : himpl::FaceLayout<Blake2s, impl::Blake2s, Hash>{digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::Blake2s(const Blake2s& from)
        : himpl::FaceLayout<Blake2s, impl::Blake2s, Hash>{from.impl()}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::Blake2s(Blake2s&& from)
        : himpl::FaceLayout<Blake2s, impl::Blake2s, Hash>{std::move(from.impl())}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::~Blake2s()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s& Blake2s::operator=(const Blake2s& from)
    {
        impl() = from.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s& Blake2s::operator=(Blake2s&& from)
    {
        impl() = std::move(from.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Blake2s::clone()
    {
        return impl().clone();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake2s::blockSize()
    {
        return impl().blockSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake2s::digestSize()
    {
        return impl().digestSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::add(const void* data, std::size_t len)
    {
        return impl().add(data, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::barrier()
    {
        return impl().barrier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::finish(void* digest)
    {
        return impl().finish(digest);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::finish(void* digest, std::size_t customDigestSize)
    {
        return impl().finish(digest, customDigestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::clear()
    {
        return impl().clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void blake2s(const void* data, std::size_t len, void* digest, std::size_t digestSize)
    {
        impl::Blake2s impl{digestSize};
        impl.add(data, len);
        impl.finish(digest);
    }
}
