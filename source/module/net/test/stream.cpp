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
#include <dci/exception.hpp>
#include <exception>
#include "dci/exception/toString.hpp"
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

struct State
{
    Manager* manager = testManager();
    Host<> netHost = manager->createService<Host<>>().value();

    stream::Server<> srv = netHost->streamServer().value();
    std::deque<stream::Client<>> srvChannels;
    stream::Client<> cln = netHost->streamClient().value();
    Endpoint srvEndpoint;

    void runServer()
    {
        EXPECT_NO_THROW((srv->listen(Ip4Endpoint{{127,0,0,1}, 0})));
        srvEndpoint = srv->localEndpoint().value();
        srv->accepted() += [this](stream::Channel<>&& srvChannel)
        {
            srvChannels.emplace_back(std::move(srvChannel));
        };
    }
};

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, stream_serverListen)
{
    State state;

    //bad address
    EXPECT_THROW((state.srv->listen(Ip4Endpoint{{192,0,2,1}, 2048})).value(), AddressNotAvailable);

    //good address
    EXPECT_NO_THROW((state.srv->listen(Ip4Endpoint{{127,0,0,1}, 0}).value()));
    EXPECT_NO_THROW((state.srv->localEndpoint().value()));
    EXPECT_EQ((state.srv->localEndpoint().value()).get<Ip4Endpoint>().address.octets, (Array<uint8, 4>{127,0,0,1}));
    EXPECT_NE((state.srv->localEndpoint().value()).get<Ip4Endpoint>().port, 0u);

    dci::utils::S2f s2fClosed{state.srv->closed()};
    state.srv->close();
    s2fClosed.wait();

    //data dropped after close
    EXPECT_NO_THROW((state.srv->localEndpoint().value()));
    EXPECT_TRUE(std::holds_alternative<NullEndpoint>(state.srv->localEndpoint().value()));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, stream_client)
{
    State state;
    state.runServer();

    //bad address
    EXPECT_NO_THROW((state.cln->bind(Ip4Endpoint{{0,0,0,0}, 0})).value());
    try
    {
        state.cln->connect(Ip4Endpoint{{255,255,255,255}, 0}).value();
    }
    catch(const AddressNotAvailable&) {}
    catch(const NetworkUnreachable&) {}
    catch(...) {GTEST_FAIL();}

    //bad address
    EXPECT_NO_THROW((state.cln->bind(state.srvEndpoint)).value());
    EXPECT_THROW((state.cln->connect(state.srvEndpoint)).value(), AddressInIse);

    //good address
    EXPECT_NO_THROW((state.cln->bind(Ip4Endpoint{{0,0,0,0}, 0})).value());
    EXPECT_NO_THROW((state.cln->connect(state.srvEndpoint).value()));
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, stream_channelClosedFailed)
{
    State state;
    state.runServer();

    sbs::Owner owner;

    int clos1 = 0;
    int clos2 = 0;
    int fail1 = 0;
    int fail2 = 0;

    stream::Channel<> ch1;
    state.srv->accepted() += owner * [&](stream::Channel<> ch)
    {
        ch1 = ch;
        ch1->closed() += owner * [&]()
        {
            clos1++;
        };
        ch1->failed() += owner * [&](auto)
        {
            fail1++;
        };
    };

    stream::Channel<> ch2 = state.cln->connect(state.srvEndpoint).value();

    while(!ch1)
    {
        sleep(1);
    }

    ch2->closed() += owner * [&]()
    {
        clos2++;
    };
    ch2->failed() += owner * [&](auto)
    {
        fail2++;
    };

    EXPECT_EQ(clos1, 0);
    EXPECT_EQ(clos2, 0);
    EXPECT_EQ(fail1, 0);
    EXPECT_EQ(fail2, 0);

    ch1->close();
    while(!clos1 || !clos2)
    {
        sleep(1);
    }
    EXPECT_EQ(fail1, 0);
    EXPECT_EQ(fail2, 0);

    ch2->close();
    sleep(1);
    EXPECT_EQ(clos1, 1);
    EXPECT_EQ(clos2, 1);
    EXPECT_EQ(fail1, 0);
    EXPECT_EQ(fail2, 0);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, stream_channelReadWrite)
{
    State state;
    state.runServer();

    sbs::Owner owner;

    int clos1 = 0;
    int clos2 = 0;
    int fail1 = 0;
    int fail2 = 0;
    int fin1 = 0;
    int fin2 = 0;

    stream::Channel<> ch1;
    state.srv->accepted() += owner * [&](stream::Channel<> ch)
    {
        ch1 = ch;
        ch1->closed() += owner * [&]()
        {
            clos1++;
        };
        ch1->failed() += owner * [&](auto)
        {
            fail1++;
        };
        ch1->received() += owner * [&](Bytes data)
        {
            EXPECT_EQ(data.size(), 1u);
            char c;
            data.begin().read(&c, 1);

            EXPECT_TRUE(c >= '0' && c <= '9');

            if(c <= '8')
            {
                char c2 = c+1;
                data.begin().write(&c2, 1);
            }
            if(c <= '9')
            {
                ch1->send(std::move(data));
            }

            if(c >= '9')
            {
                fin1++;
                ch1->close();
            }
        };
        ch1->startReceive();
    };

    stream::Channel<> ch2 = state.cln->connect(state.srvEndpoint).value();
    ch2->closed() += owner * [&]()
    {
        clos2++;
    };
    ch2->failed() += owner * [&](auto)
    {
        fail2++;
    };
    ch2->received() += owner * [&](Bytes data)
    {
        EXPECT_EQ(data.size(), 1u);
        char c;
        data.begin().read(&c, 1);

        EXPECT_TRUE(c >= '0' && c <= '9');

        if(c <= '8')
        {
            char c2 = c+1;
            data.begin().write(&c2, 1);
        }
        if(c <= '9')
        {
            ch2->send(std::move(data));
        }

        if(c >= '9')
        {
            fin2++;
            ch2->close();
        }
    };
    ch2->startReceive();

    ch2->send(Bytes{"0"});

    while(!clos1)
    {
        sleep(1);
    }
    EXPECT_EQ(clos1, 1);
    EXPECT_EQ(clos2, 1);
    EXPECT_EQ(fail1, 0);
    EXPECT_EQ(fail2, 0);
    EXPECT_EQ(fin1, 1);
    EXPECT_EQ(fin2, 1);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, stream_serverClosed)
{
    State state;
    state.runServer();

    sbs::Owner owner;

    int cnt = 0;
    state.srv->closed() += owner*[&]
    {
        cnt++;
    };

    state.srv->close();
    dci::cmt::yield();
    EXPECT_EQ(cnt, 1);

    state.srv->close();
    dci::cmt::yield();
    EXPECT_EQ(cnt, 1);
}










/*
/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_net, server)
{
    Manager* manager = testManager();
    dbgAssert(manager);

    Host<> netHost = manager->createService<Host<>>().value();

    Endpoint bindEndpoint = Ip4Endpoint {{127,0,0,1}, 0};
    Ip4Endpoint srvEndpoint;
    Event serverReady;

    stream::Server<> srv;
    stream::Client<> cl;

    cmt::spawn() += [&]
    {
        srv = netHost->streamServer().value();
        srv->listen(bindEndpoint).value();

        srvEndpoint = srv->localEndpoint().value().get<Ip4Endpoint>();
        std::cout<<"srv at: "<<srvEndpoint.port<<std::endl;
        serverReady.raise();

        srv->accepted() += [](stream::Channel<> ch)
        {
            ch->send("hello\n");
            ch->received() += [=](Bytes data) mutable
            {
                std::cout<<"srv: "<<data.toString();
                ch->send(std::move(data));
            });
        });
    };

    cmt::spawn() += [&]
    {
        cl = netHost->streamClient().value();

        serverReady.wait();

        //cl.bind(LocalEndpoint {"/tmp/221"});

        stream::Channel<> ch = cl->connect(srvEndpoint).value();
        ch->received() += [=](Bytes data) mutable
        {
            std::cout<<"cln: "<<data.toString();
            ch->send(std::move(data));
        });
    };

    dci::poll::WaitableTimer t{std::chrono::seconds{10}};
    t.start();
    t.wait();
}
//*/
