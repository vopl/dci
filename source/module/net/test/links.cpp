/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/poll.hpp>
#include <dci/utils/s2f.hpp>
#include "net.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;
using namespace dci::idl::gen::net;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, links)
{
    Manager* manager = testManager();
    Host<> netHost = manager->createService<Host<>>().value();

    cmt::waitAny(utils::S2f{netHost->linkAdded()}.waitable(), poll::timeout(std::chrono::milliseconds{100}).waitable());

    EXPECT_NO_THROW(netHost->links().value());
    EXPECT_FALSE(netHost->links().value().empty());

    auto linksMap = netHost->links().value();
    for(auto& p : linksMap)
    {
        auto l = p.second;
        EXPECT_NO_THROW(l->id().value());
        EXPECT_EQ(l->id().value(), p.first);
        EXPECT_NO_THROW(l->name().value());
        EXPECT_NO_THROW(l->flags().value());
        EXPECT_NO_THROW(l->mtu().value());
        EXPECT_NO_THROW(l->addr().value());
    }
}


















/*
std::string toHex(uint8 v)
{
    std::string res;
    uint8 h = (v >> 4) & 0xf;
    if(h < 10) res += static_cast<char>('0'+h);
    else res = static_cast<char>('a'+h-10);
    h = (v) & 0xf;
    if(h < 10) res += static_cast<char>('0'+h);
    else res += static_cast<char>('a'+h-10);

    return res;
}

std::ostream& operator<<(std::ostream& out, const Ip4Address& addr)
{
    out
        <<std::to_string(addr.octets[0])<<"."
        <<std::to_string(addr.octets[1])<<"."
        <<std::to_string(addr.octets[2])<<"."
        <<std::to_string(addr.octets[3]);

    return out;
}

std::ostream& operator<<(std::ostream& out, const Ip6Address& addr)
{
    out
        <<toHex(addr.octets[0])<<toHex(addr.octets[1])<<":"
        <<toHex(addr.octets[2])<<toHex(addr.octets[3])<<":"
        <<toHex(addr.octets[4])<<toHex(addr.octets[5])<<":"
        <<toHex(addr.octets[6])<<toHex(addr.octets[7])<<":"
        <<toHex(addr.octets[8])<<toHex(addr.octets[9])<<":"
        <<toHex(addr.octets[10])<<toHex(addr.octets[11])<<":"
        <<toHex(addr.octets[12])<<toHex(addr.octets[13])<<":"
        <<toHex(addr.octets[14])<<toHex(addr.octets[15]);

    return out;
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, links)
{
    Manager* manager = testManager();
    dbgAssert(manager);

    {
        auto linkDumper = [&](uint32 id, Link<true> link)
        {
            std::cout<<"link "<<id<<":"<<std::endl;

            std::cout<<"    id   : "<<link.id().value()    <<std::endl;
            std::cout<<"    name : "<<link.name().value()  <<std::endl;

            std::cout<<"    flags: ";
            {
                int flags = link.flags().value();

                if(flags & LinkFlags::up        ) std::cout<<"up ";
                if(flags & LinkFlags::running   ) std::cout<<"running ";
                if(flags & LinkFlags::broadcast ) std::cout<<"broadcast ";
                if(flags & LinkFlags::loopback  ) std::cout<<"loopback ";
                if(flags & LinkFlags::p2p       ) std::cout<<"p2p ";
                if(flags & LinkFlags::multicast ) std::cout<<"multicast ";
            }
            std::cout<<std::endl;

            std::cout<<"    mtu  : "<<link.mtu().value()   <<std::endl;

            List<LinkAddress> addresses = link.addr().value();

            std::cout<<"    addr : "<<addresses.size()<<":"<<std::endl;
            for(const LinkAddress& addr : addresses)
            {
                addr.visit([](const auto& v)
                {
                    if constexpr(std::is_same_v<decltype(v), const LinkIp4Address&>)
                    {
                        std::cout<<"           v4 address: "<<v.address<<", netmask: "<<v.netmask<<", broadcast: "<<v.broadcast<<std::endl;
                    }
                    if constexpr(std::is_same_v<decltype(v), const LinkIp6Address&>)
                    {
                        std::cout<<"           v6 address: "<<v.address<<", prefixLength: "<<v.prefixLength<<", scope: ";
                        switch(v.scope)
                        {
                        case LinkIp6AddressScope::null:
                            std::cout<<"null";
                            break;
                        case LinkIp6AddressScope::host:
                            std::cout<<"host";
                            break;
                        case LinkIp6AddressScope::link:
                            std::cout<<"link";
                            break;
                        case LinkIp6AddressScope::site:
                            std::cout<<"site";
                            break;
                        case LinkIp6AddressScope::global:
                            std::cout<<"global";
                            break;
                        }
                        std::cout<<std::endl;
                    }
                });
            }
        };

        Host<> netHost = manager->createService<Host<>>().value();

        auto dumpAll = [=]() mutable
        {
            std::cout<<"---------------- all"<<std::endl;
            for(auto p : netHost.links().value())
            {
                uint32 id = p.first;
                Link<true> link = p.second;

                linkDumper(id, link);
            }
        };

        auto linkTracker = [&](uint32 id, Link<true> link)
        {
            link.removed() += [=]() mutable
            {
                std::cout<<"removed link "<<id<<std::endl;
                dumpAll();
            });
            link.changed() += [=]() mutable
            {
                std::cout<<"changed link "<<id<<std::endl;
                linkDumper(id, link);

                dumpAll();
            });
        };

        netHost.linkAdded() += [&](uint32 id, Link<true> link)
        {
            std::cout<<"added ";
            linkDumper(id, link);
            linkTracker(id, link);

            dumpAll();
        });

        dumpAll();
        for(auto p : netHost.links().value())
        {
            linkTracker(p.first, p.second);
        }

//        dci::poll::Timer t{std::chrono::hours{10}};
//        t.start();
//        t.wait();
    }
}
*/
