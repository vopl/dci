/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "api.hpp"
#include <cstdint>
#include <array>
#include <string_view>

namespace dci::utils::ip
{
    enum class Scope : std::uint32_t
    {
        null         = 0,

        //ip4
        unknown4     = 1u << 0,
        host4        = 1u << 1,
        link4        = 1u << 2,
        lan4_192     = 1u << 3,
        lan4_192_168 = 1u << 4,
        lan4_198_18  = 1u << 5,
        lan4_172_16  = 1u << 6,
        lan4_100_64  = 1u << 7,
        lan4_10      = 1u << 8,
        lan4         = lan4_192 | lan4_192_168 | lan4_198_18 | lan4_172_16 | lan4_100_64 | lan4_10,
        wan4         = 1u << 9,
        ip4          = unknown4 | host4 | link4 | lan4 | wan4,

        //ip6
        unknown6 = 1u << 16,
        host6    = 1u << 17,
        link6    = 1u << 18,
        lan6     = 1u << 19,
        wan6     = 1u << 20,
        ip6      = host6 | link6 | lan6 | wan6,

        //all
        unknown  = unknown4 | unknown6,
        host     = host4    | host6,
        link     = link4    | link6,
        lan      = lan4     | lan6,
        wan      = wan4     | wan6,
    };

    using Address4 = std::array<std::uint8_t, 4>;
    using Address6 = std::array<std::uint8_t, 16>;

    using Port = std::uint16_t;
    using LinkId = std::uint32_t;

    API_DCI_UTILS Scope scope(const Address4& addr);
    API_DCI_UTILS Scope scope(const Address6& addr);
    API_DCI_UTILS Scope scope(std::string_view addr, Scope dflt = {});

    API_DCI_UTILS bool isCover(Scope base, Scope target);
    API_DCI_UTILS bool isCover(std::string_view base, std::string_view target);
    API_DCI_UTILS bool isCover(const Address4& baseIp4, std::string_view target);
    API_DCI_UTILS bool isCover(const Address6& baseIp6, ip::LinkId baseLinkId, std::string_view target);

    API_DCI_UTILS Address4 masked(const Address4& addr, const Address4& mask);
    API_DCI_UTILS Address4 masked(const Address4& addr, const Address4& mask, std::uint8_t bits);
    API_DCI_UTILS Address4 masked(const Address4& addr, std::uint8_t bits);
    API_DCI_UTILS Address6 masked(const Address6& addr, const Address6& mask);
    API_DCI_UTILS Address6 masked(const Address6& addr, const Address6& mask, std::uint8_t bits);
    API_DCI_UTILS Address6 masked(const Address6& addr, std::uint8_t bits);

    API_DCI_UTILS bool match(const Address4& addr, const Address4& mask);
    API_DCI_UTILS bool match(const Address4& addr, const Address4& mask, std::uint8_t bits);
    API_DCI_UTILS bool match(const Address6& addr, const Address6& mask);
    API_DCI_UTILS bool match(const Address6& addr, const Address6& mask, std::uint8_t bits);

    API_DCI_UTILS std::string toString(Port port);

    API_DCI_UTILS std::string toString(const Address4& addr);
    API_DCI_UTILS std::string toString(const Address4& addr, Port port);

    API_DCI_UTILS std::string toString(const Address6& addr);
    API_DCI_UTILS std::string toString(const Address6& addr, Port port);
    API_DCI_UTILS std::string toString(const Address6& addr, LinkId linkId);
    API_DCI_UTILS std::string toString(const Address6& addr, LinkId linkId, Port port);

    API_DCI_UTILS bool fromString(std::string_view str, Port& port);

    API_DCI_UTILS bool fromString(std::string_view str, Address4& addr);
    API_DCI_UTILS bool fromString(std::string_view str, Address4& addr, Port& port);

    API_DCI_UTILS bool fromString(std::string_view str, Address6& addr);
    API_DCI_UTILS bool fromString(std::string_view str, Address6& addr, Port& port);
    API_DCI_UTILS bool fromString(std::string_view str, Address6& addr, LinkId& linkId);
    API_DCI_UTILS bool fromString(std::string_view str, Address6& addr, LinkId& linkId, Port& port);
}

namespace dci::utils::ip
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr auto operator&(Scope a, Scope b)
    {
        using UT = std::underlying_type_t<Scope>;
        return static_cast<UT>(a) & static_cast<UT>(b);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr auto operator|(Scope a, Scope b)
    {
        using UT = std::underlying_type_t<Scope>;
        return static_cast<UT>(a) | static_cast<UT>(b);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr auto operator^(Scope a, Scope b)
    {
        using UT = std::underlying_type_t<Scope>;
        return static_cast<UT>(a) | static_cast<UT>(b);
    }
}
