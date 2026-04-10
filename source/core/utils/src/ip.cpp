/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/utils/ip.hpp>
#include <dci/utils/dbg.hpp>
#include <cstring>
#include <regex>
#include <charconv>

#if __has_include(<arpa/inet.h>)
#   include <arpa/inet.h>
#endif

#if __has_include(<ws2tcpip.h>)
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   include <ws2tcpip.h>
#endif

namespace dci::utils::ip
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scope scope(const Address4& addr)
    {
        if(match(addr, Address4{127,  0,  0,  0},  8)) return Scope::host4;

        if(match(addr, Address4{169,254,  0,  0}, 16)) return Scope::link4;

        if(match(addr, Address4{192,  0,  0,  0}, 24)) return Scope::lan4_192;
        if(match(addr, Address4{192,168,  0,  0}, 16)) return Scope::lan4_192_168;
        if(match(addr, Address4{198, 18,  0,  0}, 15)) return Scope::lan4_198_18;
        if(match(addr, Address4{172, 16,  0,  0}, 12)) return Scope::lan4_172_16;
        if(match(addr, Address4{100, 64,  0,  0}, 10)) return Scope::lan4_100_64;
        if(match(addr, Address4{10 ,  0,  0,  0},  8)) return Scope::lan4_10;

        if(match(addr, Address4{0  ,  0,  0,  0},  8)) return Scope::unknown4;
        if(match(addr, Address4{192, 88, 99,  0}, 24)) return Scope::unknown4;
        if(match(addr, Address4{192,  0,  2,  0}, 24)) return Scope::unknown4;
        if(match(addr, Address4{192,  0,  0,  0}, 24)) return Scope::unknown4;
        if(match(addr, Address4{198, 51,100,  0}, 24)) return Scope::unknown4;
        if(match(addr, Address4{198, 18,  0,  0}, 15)) return Scope::unknown4;
        if(match(addr, Address4{203,  0,113,  0}, 24)) return Scope::unknown4;
        if(match(addr, Address4{224,  0,  0,  0},  4)) return Scope::unknown4;
        if(match(addr, Address4{240,  0,  0,  0},  4)) return Scope::unknown4;
        if(match(addr, Address4{255,255,255,255}, 32)) return Scope::unknown4;

        return Scope::wan4;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scope scope(const Address6& addr)
    {
        if(match(addr, Address6{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1}, 128)) return Scope::host6;
        if(match(addr, Address6{254,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  10)) return Scope::link6;
        if(match(addr, Address6{252,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},   7)) return Scope::lan6;
        if(match(addr, Address6{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0},  96)) return scope(Address4{addr[12], addr[13], addr[14], addr[15]});
        if(match(addr, Address6{  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0},  96)) return scope(Address4{addr[12], addr[13], addr[14], addr[15]});
        if(match(addr, Address6{ 64,255,155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},  96)) return scope(Address4{addr[12], addr[13], addr[14], addr[15]});
        if(match(addr, Address6{255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}, 128)) return Scope::unknown6;
        if(match(addr, Address6{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, 128)) return Scope::unknown6;

        return Scope::wan6;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Scope scope(std::string_view addr, Scope dflt)
    {
        {
            Address4 addr4;
            Port port;

            if(fromString(addr, addr4, port))
                return scope(addr4);
        }

        {
            Address6 addr6;
            LinkId linkId;
            Port port;

            if(fromString(addr, addr6, linkId, port))
                return scope(addr6);
        }

        return dflt;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(Scope base, Scope target)
    {
        //
        if(base == Scope::null          ) return (target & Scope::wan);

        //4
        if(base == Scope::unknown4      ) return (target & Scope::wan);
        if(base == Scope::host4         ) return (target & Scope::wan) || (target & Scope::lan4) || (target & Scope::link4) || (target & Scope::host4);
        if(base == Scope::link4         ) return (target & Scope::wan) || (target & Scope::lan4) || (target & Scope::link4);

        if(base == Scope::lan4_192      ) return (target & Scope::wan) || (target & Scope::lan4_192);
        if(base == Scope::lan4_192_168  ) return (target & Scope::wan) || (target & Scope::lan4_192_168);
        if(base == Scope::lan4_198_18   ) return (target & Scope::wan) || (target & Scope::lan4_198_18);
        if(base == Scope::lan4_172_16   ) return (target & Scope::wan) || (target & Scope::lan4_172_16);
        if(base == Scope::lan4_100_64   ) return (target & Scope::wan) || (target & Scope::lan4_100_64);
        if(base == Scope::lan4_10       ) return (target & Scope::wan) || (target & Scope::lan4_10);
        if(base == Scope::lan4          ) return (target & Scope::wan) || (target & Scope::lan4);
        if(base == Scope::wan4          ) return (target & Scope::wan);

        //6
        if(base == Scope::unknown4      ) return (target & Scope::wan);
        if(base == Scope::host6         ) return (target & Scope::wan) || (target & Scope::lan6) || (target & Scope::link6) || (target & Scope::host6);
        if(base == Scope::link6         ) return (target & Scope::wan) || (target & Scope::lan6) || (target & Scope::link6);
        if(base == Scope::lan6          ) return (target & Scope::wan) || (target & Scope::lan6);
        if(base == Scope::wan6          ) return (target & Scope::wan);

        //4+6
        if(base == Scope::unknown       ) return (target & Scope::wan);
        if(base == Scope::host          ) return (target & Scope::wan) || (target & Scope::lan) || (target & Scope::link) || (target & Scope::host);
        if(base == Scope::link          ) return (target & Scope::wan) || (target & Scope::lan) || (target & Scope::link);
        if(base == Scope::lan           ) return (target & Scope::wan) || (target & Scope::lan);
        if(base == Scope::wan           ) return (target & Scope::wan);

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(std::string_view base, std::string_view target)
    {
        {
            Address4 addr;
            Port port;
            if(fromString(base, addr, port))
                return isCover(addr, target);
        }

        {
            Address6 addr;
            LinkId linkId;
            Port port;
            if(fromString(base, addr, linkId, port))
                return isCover(addr, linkId, target);
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(const Address4& baseIp4, std::string_view target)
    {
        Scope targetScope = scope(target, Scope::wan);
        return isCover(scope(baseIp4), targetScope);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool isCover(const Address6& baseIp6, LinkId baseLinkId, std::string_view target)
    {
        {
            Port port;
            Address6 targetIp6;
            LinkId targetLinkId;
            if(fromString(target, targetIp6, targetLinkId, port))
            {
                Scope baseScope = scope(baseIp6);
                Scope targetScope = scope(targetIp6);

                if(Scope::link6 == baseScope && Scope::link6 == targetScope)
                {
                    return baseLinkId == targetLinkId;
                }

                return isCover(baseScope, targetScope);
            }
        }

        {
            Address4 targetIp4;
            if(fromString(target, targetIp4))
            {
                Scope baseScope = scope(baseIp6);
                Scope targetScope = scope(targetIp4);
                return isCover(baseScope, targetScope);
            }
        }

        //парс не удался, предполагаю что там доменное имя
        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        template <size_t len>
        std::array<std::uint8_t, len> maskedImpl(
                const std::array<std::uint8_t, len>& addr,
                const std::array<std::uint8_t, len>& mask)
        {
            std::array<std::uint8_t, len> masked;
            for(std::size_t i{0}; i<len; ++i)
            {
                masked[i] = addr[i] & mask[i];
            }
            return masked;
        }

        template <size_t len>
        std::array<std::uint8_t, len> maskedImpl(
                const std::array<std::uint8_t, len>& addr,
                const std::array<std::uint8_t, len>& mask,
                std::size_t bits)
        {
            std::array<std::uint8_t, len> masked {};

            size_t idx = 0;
            while(bits >= 8)
            {
                masked[idx] = addr[idx] & mask[idx];
                bits -= 8;
                idx++;
            }

            if(bits)
            {
                bits = 8-bits;
                std::uint8_t mask2 = ~std::uint8_t((1<<bits) - 1);

                masked[idx] = addr[idx] & mask[idx] & mask2;
            }

            return masked;
        }

        template <size_t len>
        std::array<std::uint8_t, len> maskedImpl(
                const std::array<std::uint8_t, len>& addr,
                std::size_t bits)
        {
            std::array<std::uint8_t, len> masked {};

            size_t idx = 0;
            while(bits >= 8)
            {
                masked[idx] = addr[idx];
                bits -= 8;
                idx++;
            }

            if(bits)
            {
                bits = 8-bits;
                std::uint8_t mask2 = ~std::uint8_t((1<<bits) - 1);

                masked[idx] = addr[idx] & mask2;
            }

            return masked;
        }

        template <size_t len>
        bool matchImpl(
                const std::array<std::uint8_t, len>& addr,
                const std::array<std::uint8_t, len>& mask)
        {
            return mask == maskedImpl(addr, mask);
        }

        template <size_t len>
        bool matchImpl(
                const std::array<std::uint8_t, len>& addr,
                const std::array<std::uint8_t, len>& mask,
                std::size_t bits)
        {
            return mask == maskedImpl(addr, bits);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Address4 masked(const Address4& addr, const Address4& mask)
    {
        return maskedImpl(addr, mask);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Address4 masked(const Address4& addr, const Address4& mask, std::uint8_t bits)
    {
        return maskedImpl(addr, mask, bits);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Address4 masked(const Address4& addr, std::uint8_t bits)
    {
        return maskedImpl(addr, bits);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Address6 masked(const Address6& addr, const Address6& mask)
    {
        return maskedImpl(addr, mask);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Address6 masked(const Address6& addr, const Address6& mask, std::uint8_t bits)
    {
        return maskedImpl(addr, mask, bits);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Address6 masked(const Address6& addr, std::uint8_t bits)
    {
        return maskedImpl(addr, bits);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool match(const Address4& addr, const Address4& mask)
    {
        return matchImpl(addr, mask);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool match(const Address4& addr, const Address4& mask, std::uint8_t bits)
    {
        return matchImpl(addr, mask, bits);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool match(const Address6& addr, const Address6& mask)
    {
        return matchImpl(addr, mask);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool match(const Address6& addr, const Address6& mask, std::uint8_t bits)
    {
        return matchImpl(addr, mask, bits);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(Port port)
    {
        return port ? std::to_string(port) : std::string{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const Address4& addr)
    {
        char buf[INET_ADDRSTRLEN] = {};
        if(!inet_ntop(AF_INET, addr.data(), buf, sizeof(buf)))
        {
            return {};
        }

        return {buf};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const Address4& addr, Port port)
    {
        return toString(addr) + (port ? ":" + std::to_string(port) : std::string{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const Address6& addr)
    {
        char buf[INET6_ADDRSTRLEN] = {};
        if(!inet_ntop(AF_INET6, addr.data(), buf, sizeof(buf)))
        {
            return {};
        }

        return {buf};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const Address6& addr, Port port)
    {
        return "[" + toString(addr) + "]" + (port ? ":" + std::to_string(port) : std::string{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const Address6& addr, LinkId linkId)
    {
        std::string res = toString(addr);

        if(linkId)
        {
            res += "%" + std::to_string(linkId);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string toString(const Address6& addr, LinkId linkId, Port port)
    {
        return "[" + toString(addr, linkId) + "]" + (port ? ":" + std::to_string(port) : std::string{});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Port& port)
    {
        if(str.empty())
        {
            port = 0;
            return true;
        }

        return std::errc{} == std::from_chars(str.data(), str.data()+str.size(), port).ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Address4& addr)
    {
        if(str.data()[str.size()])//asciiz?
        {
            return 1 == inet_pton(AF_INET, str.data(), addr.data());
        }

        char buf[INET_ADDRSTRLEN] = {};
        std::memcpy(buf, str.data(), std::min(std::size_t{INET_ADDRSTRLEN}, str.size()));
        return 1 == inet_pton(AF_INET, buf, addr.data());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Address4& addr, Port& port)
    {
        std::match_results<std::string_view::const_iterator> match;
        static const std::regex regex("^([^\\:]+)(?:\\:(.*))?$", std::regex::optimize);
        if(!std::regex_match(str.begin(), str.end(), match, regex))
        {
            return false;
        }

        std::sub_match<std::string_view::const_iterator> sm = match[1];
        std::string_view smStr{sm.first, sm.second};
        if(!sm.matched || !fromString(smStr, addr))
        {
            return false;
        }

        sm = match[2];
        smStr = std::string_view{sm.first, sm.second};
        if(!sm.matched || sm.first == sm.second)
        {
            port = 0;
            return true;
        }

        return std::errc{} == std::from_chars(smStr.data(), smStr.data()+smStr.size(), port).ec;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Address6& addr)
    {
        if(str.data()[str.size()])//asciiz?
        {
            return 1 == inet_pton(AF_INET6, str.data(), addr.data());
        }

        char buf[INET6_ADDRSTRLEN] = {};
        std::memcpy(buf, str.data(), std::min(std::size_t{INET6_ADDRSTRLEN}, str.size()));
        return 1 == inet_pton(AF_INET6, buf, addr.data());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Address6& addr, Port& port)
    {
        LinkId linkId;
        return fromString(str, addr, linkId, port);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Address6& addr, LinkId& linkId)
    {
        Port port;
        return fromString(str, addr, linkId, port);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool fromString(std::string_view str, Address6& addr, LinkId& linkId, Port& port)
    {
        std::match_results<std::string_view::const_iterator> match;
        static const std::regex regex("^(?:\\[)?([0-9a-fA-F:]*)(?:%([0-9]*))?(?:\\]:(.*))?$", std::regex::optimize);

        if(!std::regex_match(str.begin(), str.end(), match, regex))
        {
            return false;
        }

        //addr
        std::sub_match<std::string_view::const_iterator> sm = match[1];
        std::string_view smStr{sm.first, sm.second};
        if(!sm.matched || !fromString(smStr, addr))
        {
            return false;
        }

        //linkId
        sm = match[2];
        smStr = std::string_view{sm.first, sm.second};
        if(!sm.matched || sm.first == sm.second)
        {
            linkId = 0;
        }
        else if(std::errc{} != std::from_chars(smStr.data(), smStr.data()+smStr.size(), port).ec)
        {
            return false;
        }

        //port
        sm = match[3];
        smStr = std::string_view{sm.first, sm.second};
        if(!sm.matched || sm.first == sm.second)
        {
            port = 0;
        }
        else if(std::errc{} != std::from_chars(smStr.data(), smStr.data()+smStr.size(), port).ec)
        {
            return false;
        }

        return true;
    }
}
