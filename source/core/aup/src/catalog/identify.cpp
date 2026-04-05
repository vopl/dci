/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/aup/catalog/release.hpp>
#include <dci/aup/catalog/file.hpp>
#include <dci/aup/catalog/object.hpp>
#include <dci/aup/catalog/unit.hpp>
#include <dci/aup/catalog/identify.hpp>
#include <dci/aup/exception.hpp>
#include <dci/crypto/blake3.hpp>
#include <dci/stiac/serialization.hpp>

#include "../impl/catalog/enumerateObjectFields.hpp"

namespace dci::aup::catalog
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        struct OidMaker
        {
            crypto::Blake3 _hashier{32};

            void write(const void* data, uint32 size)
            {
                _hashier.add(data, size);
            }

            OidMaker& operator<<(auto&& v)
            {
                using stiac::serialization::save;

                save(*this, std::forward<decltype(v)>(v));
                return *this;
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid identify(const Bytes& blob)
    {
        OidMaker arch;

        bytes::Cursor c{blob.begin()};

        while(!c.atEnd())
        {
            arch.write(c.continuousData(), c.continuousDataSize());
            c.advanceChunks(1);
        }

        Oid res;
        dbgAssert(res.size() == arch._hashier.digestSize());
        arch._hashier.finish(res.data());

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid identify(std::FILE* f)
    {
        OidMaker arch;

        rewind(f);
        char buf[1024];

        for(;;)
        {
            std::size_t s = fread(buf, 1, sizeof(buf), f);
            if(!s)
            {
                break;
            }

            arch.write(buf, static_cast<uint32>(s));

            if(s != sizeof(buf))
            {
                break;
            }
        }

        Oid res;
        dbgAssert(res.size() == arch._hashier.digestSize());
        arch._hashier.finish(res.data());

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid identify(const aup::catalog::Object* object)
    {
        OidMaker arch;
        arch << object->type();

        impl::catalog::enumerateObjectFields(object, [&](const auto& fld)
        {
            arch << fld;
        });

        Oid res;
        dbgAssert(res.size() == arch._hashier.digestSize());
        arch._hashier.finish(res.data());

        return res;
    }
}
