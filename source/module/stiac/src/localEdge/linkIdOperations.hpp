/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../pch.hpp"

namespace dci::module::stiac::localEdge
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr bool linkIsNull(link::Id id)
    {
        return link::Id::null == id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr bool linkIsLocal(link::Id id)
    {
        int64 u = static_cast<int64>(id);
        return u > 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr bool linkIsRemote(link::Id id)
    {
        int64 u = static_cast<int64>(id);
        return u < 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr bool linkIsNull(link::LocalId id)
    {
        return link::LocalId::null == id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr bool linkIsNull(link::RemoteId id)
    {
        return link::RemoteId::null == id;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Dst, class Src> constexpr Dst linkIdCast(Src);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    constexpr link::LocalId linkIdCast<link::LocalId>(link::Id src)
    {
        if(!linkIsLocal(src)) throw "bad linkId cast";
        int64 u = static_cast<int64>(src);
        return static_cast<link::LocalId>(u);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    constexpr link::RemoteId linkIdCast(link::Id src)
    {
        if(!linkIsRemote(src)) throw "bad linkId cast";
        int64 u = static_cast<int64>(src);
        return static_cast<link::RemoteId>(-u);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    constexpr link::Id linkIdCast<link::Id>(link::LocalId src)
    {
        if(linkIsNull(src)) throw "bad linkId cast";
        uint64 u = static_cast<uint64>(src);
        return static_cast<link::Id>(u);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    constexpr link::Id linkIdCast<link::Id>(link::RemoteId src)
    {
        if(linkIsNull(src)) throw "bad linkId cast";
        uint64 u = static_cast<uint64>(src);
        return static_cast<link::Id>(-int64(u));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    constexpr link::Id linkIdCast(link::MirroredId src)
    {
        int64 u = static_cast<int64>(src);
        return static_cast<link::Id>(-u);
    }

}
