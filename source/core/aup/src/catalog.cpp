/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/aup/catalog.hpp>
#include "impl/catalog.hpp"

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::Catalog()
        : himpl::FaceLayout<Catalog, impl::Catalog>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Catalog::~Catalog()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::deserialize(Bytes&& blob)
    {
        return impl().deserialize(std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes Catalog::serialize()
    {
        return impl().serialize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<Oid> Catalog::enumerate(catalog::Object::Type type)
    {
        return impl().enumerate(type);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Oid Catalog::put(catalog::ObjectPtr&& object)
    {
        return impl().put(std::move(object));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Catalog::has(const Oid& oid)
    {
        return impl().has(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    catalog::ObjectPtr Catalog::get(const Oid& oid)
    {
        return impl().get(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Catalog::del(const Oid& oid)
    {
        return impl().del(oid);
    }
}
