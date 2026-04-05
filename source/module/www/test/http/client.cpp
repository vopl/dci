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
#include <dci/cmt.hpp>
#include <dci/exception.hpp>
#include "www.hpp"
#include "utils.hpp"
#include "spectacle.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;
using namespace dci::idl;
using namespace dci::primitives;
using namespace http;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, noInput)
{
    SpectacleForClient spectacle;
    spectacle.startIo();
    spectacle.play();

    ASSERT_EQ(spectacle._actions.size(), 1);
    ASSERT_EQ(spectacle._actions[0], SpectacleForClient::Io{});
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, smallVersion)
{
    auto sample = [](Bytes traffic)
    {
        SpectacleForClient spectacle;
        spectacle.startIo();
        spectacle._peer->send(traffic);
        spectacle.play();

        ASSERT_EQ(spectacle._actions.size(), 1);
        ASSERT_EQ(spectacle._actions[0], SpectacleForClient::Io{});
    };

    sample("x");
    sample("0123456789");
    sample("0123456789012345");// less then 16
    sample("HTTP/1.1________");// less then 16
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, bigVersion)
{
    CLIENT_PLAY_2_FAIL("012345678901234567", response::BadResponse);
    CLIENT_PLAY_2_FAIL("HTTP/1.1_________e", response::BadResponse);
    CLIENT_PLAY_2_FAIL("HTTP/1\t\v\b\0______e", response::BadResponse);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, badVersion)
{
    CLIENT_PLAY_2_FAIL("hTTP/1.1 200 OK\r\n", response::BadVersion);
    CLIENT_PLAY_2_FAIL("h 200 OK\r\n", response::BadVersion);
    CLIENT_PLAY_2_FAIL("\0 200 OK\r\n", response::BadVersion);
    CLIENT_PLAY_2_FAIL("\b\n\r 200 OK\r\n", response::BadVersion);
    CLIENT_PLAY_2_FAIL("HTTP/1.2 200 OK\r\n", response::BadVersion);
    CLIENT_PLAY_2_FAIL("HTTP/2.0 200 OK\r\n", response::BadVersion);
    CLIENT_PLAY_2_FAIL("HTTP/2 200 OK\r\n", response::BadVersion);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, badStatusCode)
{
    CLIENT_PLAY_2_FAIL("HTTP/1.1 9 Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 99 Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 1000 Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 A Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 aA Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 waA Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 2A Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 20A Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 200A Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 \00A Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 \b0A Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 \t Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 \t\b\b\b Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 \t\0 Bla\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 \r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 2\r\n", response::BadStatus);
    CLIENT_PLAY_2_FAIL("HTTP/1.1 200\r\n", response::BadStatus);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, badStatusText)
{
    CLIENT_PLAY_2_FAIL("HTTP/1.1 200 "+std::string(65, 'x')+"\r\n", response::BadResponse);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, one)
{
    SpectacleForClient spectacle;
    spectacle.startIo();
    spectacle._peer->send("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n012");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 1);
    ASSERT_TRUE(spectacle.has<SpectacleForClient::Io>());
    ASSERT_TRUE(spectacle.has<SpectacleForClient::InputFirstLine4C>());
    ASSERT_TRUE(spectacle.has<SpectacleForClient::InputHeaders>());
    ASSERT_TRUE(spectacle.has<SpectacleForClient::InputData>());
    ASSERT_TRUE(spectacle.has<SpectacleForClient::InputDone>());

    ASSERT_FALSE(spectacle.has<SpectacleForClient::Failed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::Closed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::InputFailed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::InputClosed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::InputFirstLine4S>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::OutputFailed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::OutputClosed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::PeerFailed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::PeerClosed>());
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, multiple)
{
    SpectacleForClient spectacle;
    spectacle.startIo();
    spectacle.startIo();
    spectacle.startIo();
    spectacle.startIo();
    spectacle.startIo();
    spectacle._peer->send("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\n012");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 5);
    ASSERT_EQ(spectacle.count<SpectacleForClient::Io>(), 5);
    ASSERT_EQ(spectacle.count<SpectacleForClient::InputFirstLine4C>(), 5);
    ASSERT_EQ(spectacle.count<SpectacleForClient::InputHeaders>(), 5);
    ASSERT_EQ(spectacle.count<SpectacleForClient::InputData>(), 5);
    ASSERT_EQ(spectacle.count<SpectacleForClient::InputDone>(), 5);

    ASSERT_FALSE(spectacle.has<SpectacleForClient::Failed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::Closed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::InputFailed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::InputClosed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::InputFirstLine4S>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::OutputFailed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::OutputClosed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::PeerFailed>());
    ASSERT_FALSE(spectacle.has<SpectacleForClient::PeerClosed>());
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, out)
{
    SpectacleForClient spectacle;
    spectacle.startIo();

    ASSERT_EQ(spectacle._ios.size(), 1);

    www::http::client::Response input = spectacle._ios[0]._input;
    www::http::client::Request output = spectacle._ios[0]._output;

    output->firstLine(www::http::firstLine::Method::PATCH, "/u/r/ii", www::http::firstLine::Version::HTTP_0_9);
    output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Content_Length, "3"}}, false);
    output->headers(List<www::http::Header>{www::http::Header{"xyz", "qwe"}}, true);
    output->data("012", true);
    output->done();
    spectacle.play();

    ASSERT_EQ(spectacle._peerDataMerged.toString(), "PATCH /u/r/ii HTTP/0.9\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_client, outMultiple)
{
    SpectacleForClient spectacle;
    spectacle.startIo();
    spectacle.startIo();
    spectacle.startIo();

    ASSERT_EQ(spectacle._ios.size(), 3);

    auto requestOne = [&](std::size_t idx)
    {
        www::http::client::Response input = spectacle._ios[idx]._input;
        www::http::client::Request output = spectacle._ios[idx]._output;

        output->firstLine(www::http::firstLine::Method::PATCH, "/u/r/ii", www::http::firstLine::Version::HTTP_0_9);
        output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Content_Length, "3"}}, false);
        output->headers(List<www::http::Header>{www::http::Header{"xyz", "qwe"}}, true);
        output->data("012", true);
        output->done();
    };
    requestOne(1);
    requestOne(2);
    requestOne(0);

    spectacle.play();

    ASSERT_EQ(spectacle._peerDataMerged.toString(),
              "PATCH /u/r/ii HTTP/0.9\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012"
              "PATCH /u/r/ii HTTP/0.9\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012"
              "PATCH /u/r/ii HTTP/0.9\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012");
}
