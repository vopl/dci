/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::natt::mapper::pmpPcp::msg
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace v0
    {
        enum Opcode : uint8
        {
            ANNOUNCE    = 0,
            MAP_UDP     = 1,
            MAP_TCP     = 2,
        };

        enum ResultCode : uint16
        {
            SUCCESS                 = 0,
            UNSUPP_VERSION          = 1,
            NOT_AUTHORIZED          = 2,
            NETWORK_FAILURE         = 3,
            OUT_OF_RESOURCES        = 4,
            UNSUPP_OPCODE           = 5,
        };

        using Address   = std::array<uint8, 4>;

#pragma pack(push,1)

        struct CommonHeader
        {
            uint8       _version        {};
            uint8       _opcode:7       {};
            uint8       _isResponse:1   {};
        };

        struct RequestHeader : CommonHeader
        {
        };

        struct ResponseHeader : CommonHeader
        {
            uint16 _resultCode      {};
            uint32 _epochTime       {};
        };

        struct Request_ANNOUNCE : RequestHeader
        {
        };

        struct Response_ANNOUNCE : ResponseHeader
        {
            Address _externalAddress{};
        };

        struct Request_MAP : RequestHeader
        {
            uint16 _reserved        {};
            uint16 _internalPort    {};
            uint16 _externalPort    {};
            uint32 _lifeTime        {};
        };

        struct Response_MAP : ResponseHeader
        {
            uint16 _internalPort    {};
            uint16 _externalPort    {};
            uint32 _lifeTime        {};
        };

#pragma pack(pop)

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace v1
    {
        enum Opcode : uint8
        {
            ANNOUNCE    = 0,
            MAP         = 1,
            //PEER        = 2,
        };

        enum ResultCode : uint8
        {
            SUCCESS                 = 0,
            UNSUPP_VERSION          = 1,
            NOT_AUTHORIZED          = 2,
            MALFORMED_REQUEST       = 3,
            UNSUPP_OPCODE           = 4,
            UNSUPP_OPTION           = 5,
            MALFORMED_OPTION        = 6,
            NETWORK_FAILURE         = 7,
            NO_RESOURCES            = 8,
            UNSUPP_PROTOCOL         = 9,
            USER_EX_QUOTA           = 10,
            CANNOT_PROVIDE_EXTERNAL = 11,
            ADDRESS_MISMATCH        = 12,
            EXCESSIVE_REMOTE_PEERS  = 13,
        };

        using Address   = std::array<uint8, 16>;

#pragma pack(push,1)

        struct CommonHeader
        {
            uint8       _version        {};
            uint8       _opcode:7       {};
            uint8       _isResponse:1   {};
            uint8       _reserved       {};
            ResultCode  _resultCode     {};
            uint32      _lifeTime       {};
        };

        struct RequestHeader : CommonHeader
        {
            Address _clientAddress {};
        };

        struct ResponseHeader : CommonHeader
        {
            uint32 _epochTime       {};
            uint8 _reserved2[12]    {};
        };

        struct Request_ANNOUNCE : RequestHeader
        {
        };

        struct Response_ANNOUNCE : ResponseHeader
        {
        };

        struct Request_MAP : RequestHeader
        {
            uint8   _protocol           {};
            uint8   _reserved[3]        {};
            uint16  _internalPort       {};
            uint16  _externalPort       {};
            Address _externalAddress    {};
        };

        struct Response_MAP : ResponseHeader
        {
            uint8   _protocol           {};
            uint8   _reserved[3]        {};
            uint16  _internalPort       {};
            uint16  _externalPort       {};
            Address _externalAddress    {};
        };

#pragma pack(pop)

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace v2
    {
        using Nonce     = std::array<uint8, 12>;

#pragma pack(push,1)

        struct Request_MAP : v1::RequestHeader
        {
            Nonce       _nonce              {};
            uint8       _protocol           {};
            uint8       _reserved[3]        {};
            uint16      _internalPort       {};
            uint16      _externalPort       {};
            v1::Address _externalAddress    {};
        };

        struct Response_MAP : v1::ResponseHeader
        {
            Nonce       _nonce              {};
            uint8       _protocol           {};
            uint8       _reserved[3]        {};
            uint16      _internalPort       {};
            uint16      _externalPort       {};
            v1::Address _externalAddress    {};
        };

#pragma pack(pop)

    }
}
