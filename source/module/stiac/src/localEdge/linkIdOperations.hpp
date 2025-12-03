// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
