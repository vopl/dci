/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/aup/storage.hpp>
#include "impl/storage.hpp"

namespace dci::aup
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::Storage()
        : himpl::FaceLayout<Storage, impl::Storage>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Storage::~Storage()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::reset(const std::string& place, bool autoFixIfCan)
    {
        return impl().reset(place, autoFixIfCan);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Set<Oid> Storage::enumerate()
    {
        return impl().enumerate();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const std::string& localPath, Bytes&& blob)
    {
        return impl().put(localPath, std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has(const std::string& localPath)
    {
        return impl().has(localPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get(const std::string& localPath, uint32 from, uint32 to)
    {
        return impl().get(localPath, from, to);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del(const std::string& localPath)
    {
        return impl().del(localPath);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::put(const Oid& oid, Bytes&& blob)
    {
        return impl().put(oid, std::move(blob));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::has(const Oid& oid)
    {
        return impl().has(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::optional<Bytes> Storage::get(const Oid& oid, uint32 from, uint32 to)
    {
        return impl().get(oid, from, to);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Storage::del(const Oid& oid)
    {
        return impl().del(oid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Storage::delAll(bool andPlaceDirectory)
    {
        return impl().delAll(andPlaceDirectory);
    }
}
