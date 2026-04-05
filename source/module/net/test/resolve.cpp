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
#include "net.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;
using namespace dci::idl::gen::net;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, resolve)
{
    Manager* manager = testManager();
    dbgAssert(manager);

    Host<> netHost = manager->createService<Host<>>().value();

    //ip4 lo
    {
        EXPECT_NO_THROW(netHost->resolveIp("127.0.0.1").value());
        IpEndpoint ep = netHost->resolveIp("127.0.0.1").value();
        EXPECT_TRUE(std::holds_alternative<Ip4Endpoint>(ep));

        Ip4Endpoint ep4 = ep.get<Ip4Endpoint>();
        EXPECT_EQ((Array<uint8, 4>{127,0,0,1}), ep4.address.octets);
        EXPECT_EQ(0, ep4.port);
    }

    //ip6 lo
    {
        EXPECT_NO_THROW(netHost->resolveIp("::1").value());
        IpEndpoint ep = netHost->resolveIp("::1").value();
        EXPECT_TRUE(std::holds_alternative<Ip6Endpoint>(ep));

        Ip6Endpoint ep6 = ep.get<Ip6Endpoint>();
        EXPECT_EQ((Array<uint8, 16>{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}), ep6.address.octets);
        EXPECT_EQ(0, ep6.port);
    }

    //internet
    {
        EXPECT_THROW(netHost->resolveIp("ajklsdfq.dsbhwerg.xvpio.er").value(), ResolveError);
    }

    //port
    {
        IpEndpoint ep = netHost->resolveIp("1.2.3.4:42").value();
        EXPECT_TRUE(std::holds_alternative<Ip4Endpoint>(ep));
        EXPECT_EQ(42, ep.get<Ip4Endpoint>().port);

        ep = netHost->resolveIp("[::]:43").value();
        EXPECT_TRUE(std::holds_alternative<Ip6Endpoint>(ep));
        EXPECT_EQ(43, ep.get<Ip6Endpoint>().port);
    }
}





/*
inline std::string toHex(uint8 v)
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

inline std::ostream& operator<<(std::ostream& out, const Ip4Address& addr)
{
    out
        <<std::to_string(addr.octets[0])<<"."
        <<std::to_string(addr.octets[1])<<"."
        <<std::to_string(addr.octets[2])<<"."
        <<std::to_string(addr.octets[3]);

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const Ip6Address& addr)
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

std::ostream& operator<<(std::ostream& out, const IpAddress& addr)
{
    addr.visit([&](const auto& addr2)
    {
        out<<addr2;
    });

    return out;
}

std::ostream& operator<<(std::ostream& out, const List<auto>& addrs)
{
    for(const auto& one : addrs)
    {
        out<<one<<" ";
    }

    return out;
}


/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, resolve)
{
    Manager* manager = testManager();
    dbgAssert(manager);

    Host<> netHost = manager->createService<Host<>>().value();

    std::cout<<netHost.resolveIp("127.0.0.1").value()<<std::endl;
    std::cout<<netHost.resolveIp("::1").value()<<std::endl;
    std::cout<<netHost.resolveIp("example.com").value()<<std::endl;

    std::cout<<netHost.resolveIp4("127.0.0.1").value()<<std::endl;
    std::cout<<netHost.resolveIp4("example.com").value()<<std::endl;

    std::cout<<netHost.resolveIp6("::1").value()<<std::endl;
    std::cout<<netHost.resolveIp6("example.com").value()<<std::endl;

    std::cout<<netHost.resolveAllIp("127.0.0.1").value()<<std::endl;
    std::cout<<netHost.resolveAllIp("::1").value()<<std::endl;
    std::cout<<netHost.resolveAllIp("example.com").value()<<std::endl;

    std::cout<<netHost.resolveAllIp4("127.0.0.1").value()<<std::endl;
    std::cout<<netHost.resolveAllIp4("example.com").value()<<std::endl;

    std::cout<<netHost.resolveAllIp6("::1").value()<<std::endl;
    std::cout<<netHost.resolveAllIp6("example.com").value()<<std::endl;

    std::cout<<netHost.resolveAllIp("github.com").value()<<std::endl;
    std::cout<<netHost.resolveAllIp("google.ru").value()<<std::endl;
    std::cout<<netHost.resolveAllIp("google.com").value()<<std::endl;

    std::cout<<netHost.resolveAllIp("localhost").value()<<std::endl;
}
*/
