/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/stiac/link/source.hpp>
#include "impl/source.hpp"

namespace dci::stiac::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source::Source(Hub4Source* hub, bytes::Alter&& buffer, bool doFinalization)
        : himpl::FaceLayout<Source, impl::Source>(hub, std::move(buffer), doFinalization)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Source::~Source()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hub4Source* Source::context()
    {
        return impl().context();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Source::size() const
    {
        return impl().size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::read(void* data, uint32 size)
    {
        return impl().read(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::read(Bytes& data, uint32 size)
    {
        return impl().read(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::pushPtr(std::type_index typeIndex, const Ptr<void>& v)
    {
        return impl().pushPtr(typeIndex, v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::pair<std::type_index, Ptr<void>> Source::getPtr(uint32 idx)
    {
        return impl().getPtr(idx);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid)
    {
        return impl().mapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid)
    {
        return impl().unmapTuid(mapped, tuid);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Source::finalize()
    {
        return impl().finalize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::finalized() const
    {
        return impl().finalized();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    [[noreturn]] void Source::fail(const char* cszDetails)
    {
        impl().fail(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Source::emplaceLink(BasePtr&& link, RemoteId id)
    {
        return impl().emplaceLink(std::move(link), id);
    }

}
