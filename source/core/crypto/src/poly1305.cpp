/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/crypto/poly1305.hpp>
#include "impl/poly1305.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Poly1305::alloc()
    {
        return HashPtr
        {
            new Poly1305,
            [](Hash* p){delete static_cast<Poly1305*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Poly1305::Poly1305()
        : himpl::FaceLayout<Poly1305, impl::Poly1305, Mac>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Poly1305::Poly1305(const Poly1305& from)
        : himpl::FaceLayout<Poly1305, impl::Poly1305, Mac>{from.impl()}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Poly1305::Poly1305(Poly1305&& from)
        : himpl::FaceLayout<Poly1305, impl::Poly1305, Mac>{std::move(from.impl())}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Poly1305::~Poly1305()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Poly1305& Poly1305::operator=(const Poly1305& from)
    {
        impl() = from.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Poly1305& Poly1305::operator=(Poly1305&& from)
    {
        impl() = std::move(from.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Poly1305::clone()
    {
        return impl().clone();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Poly1305::setKey(const void* key, std::size_t len)
    {
        return impl().setKey(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Poly1305::blockSize()
    {
        return impl().blockSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Poly1305::digestSize()
    {
        return impl().digestSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Poly1305::add(const void* data, std::size_t len)
    {
        return impl().add(data, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Poly1305::barrier()
    {
        return impl().barrier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Poly1305::finish(void* digest)
    {
        return impl().finish(digest);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Poly1305::finish(void* digest, std::size_t customDigestSize)
    {
        return impl().finish(digest, customDigestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Poly1305::clear()
    {
        return impl().clear();
    }
}
