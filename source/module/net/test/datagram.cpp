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

namespace
{
    void sleep(int ms)
    {
        dci::poll::WaitableTimer t{std::chrono::milliseconds{ms}};
        t.start();
        t.wait();
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, datagram_ping_pong)
{
    Manager* manager = testManager();
    Host<> netHost = manager->createService<Host<>>().value();

    datagram::Channel<> ch1 = netHost->datagramChannel().value();
    datagram::Channel<> ch2 = netHost->datagramChannel().value();

    Ip4Endpoint ep1{{127,0,0,1}, 1818};
    Ip4Endpoint ep2{{127,0,0,1}, 1819};

    EXPECT_NO_THROW((ch1->bind(ep1).value()));
    EXPECT_NO_THROW((ch2->bind(ep2).value()));

    int cnt1 = 0;
    int cnt2 = 0;

    ch1->received() += [&](Bytes data, Endpoint from)
    {
        EXPECT_EQ(1u, from.index());
        EXPECT_EQ(1819u, from.get<Ip4Endpoint>().port);
        EXPECT_EQ((Array<uint8, 4>{127,0,0,1}), from.get<Ip4Endpoint>().address.octets);

        EXPECT_EQ(data.toString(), "for ch1");

        cnt1++;
        ch1->send(Bytes("for ch2"), ep2);
    };

    ch2->received() += [&](Bytes data, Endpoint from)
    {
        EXPECT_EQ(1u, from.index());
        EXPECT_EQ(1818u, from.get<Ip4Endpoint>().port);
        EXPECT_EQ((Array<uint8, 4>{127,0,0,1}), from.get<Ip4Endpoint>().address.octets);

        EXPECT_EQ(data.toString(), "for ch2");

        cnt2++;
        ch2->send(Bytes("for ch1"), ep1);
    };

    ch1->send(Bytes("for ch2"), ep2);
    ch2->send(Bytes("for ch1"), ep1);

    while(cnt1<10 && cnt2<10)
    {
        sleep(1);
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, datagram_large_message)
{
    Manager* manager = testManager();
    Host<> netHost = manager->createService<Host<>>().value();

    datagram::Channel<> ch1 = netHost->datagramChannel().value();
    datagram::Channel<> ch2 = netHost->datagramChannel().value();

    Ip4Endpoint ep1{{127,0,0,1}, 1818};
    Ip4Endpoint ep2{{127,0,0,1}, 1819};

    EXPECT_NO_THROW((ch1->bind(ep1).value()));
    EXPECT_NO_THROW((ch2->bind(ep2).value()));

    int cnt = 0;

    ch1->received() += [&](Bytes data, Endpoint /*from*/)
    {
        EXPECT_EQ(data.size(), 1024u*37);
        cnt++;
    };

    Bytes data;
    data.begin().advance(1024*37);
    ch2->send(data, ep1);

    for(int i(0); i<100 && !cnt; ++i)
    {
        sleep(1);
    }

    EXPECT_EQ(1, cnt);
}
