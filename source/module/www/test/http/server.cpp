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
using namespace std::literals;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, noInput)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("");
    spectacle.play();

    ASSERT_EQ(spectacle._actions.size(), 0);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, smallMethod)
{
    auto sample = [](Bytes traffic)
    {
        SpectacleForServer spectacle;
        spectacle._peer->send(traffic);
        spectacle.play();

        ASSERT_EQ(spectacle._actions.size(), 1);
        ASSERT_EQ(spectacle._actions[0], SpectacleForServer::Io{});
    };

    sample("x");
    sample("0123456789");
    sample("01234567890123456789012345678901");// less then 32
    sample("GET_____________________________");// less then 32
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bigMethod)
{
    SERVER_PLAY_2_FAIL("01234567890123456789012345678901e",        request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET_____________________________e",        request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET_\t__\v_\b___\0_____________________e", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, badMethod)
{
    SERVER_PLAY_2_FAIL("TEG uri HTTP/1.1\r\n", request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("TE uri HTTP/1.1\r\n",  request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("T uri HTTP/1.1\r\n",   request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("XYZ uri HTTP/1.1\r\n", request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("1 uri HTTP/1.1\r\n",   request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL(" uri HTTP/1.1\r\n",    request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("# uri HTTP/1.1\r\n",   request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("! uri HTTP/1.1\r\n",   request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("\330 uri HTTP/1.1\r\n",request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("\n uri HTTP/1.1\r\n",  request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("( uri HTTP/1.1\r\n",   request::BadMethod, "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bigUri)
{
    SERVER_PLAY_2_FAIL("METH " + std::string(8193, 'x'), request::TooBigUri, "HTTP/1.1 414 URI Too Long\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bigVersion)
{
    SERVER_PLAY_2_FAIL("GET uri HTTP/012345678901\r\n", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, badRequestByVersion)
{
    SERVER_PLAY_2_FAIL("GET uri \t\r\n",                       request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri 0123\r\n",                     request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri \x01\xd4\x10\x21\x02\x02\r\n", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTZ/1.1\r\n",                 request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri http/1.1\r\n",                 request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri http/1.1\r\n",                 request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, badVersion)
{
    SERVER_PLAY_2_FAIL("GET uri HTTP/\x00\x01\r\n",        request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/\x01\x21\r\n",        request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/\x02\x01\x01\x01\r\n",request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/\x07\x07\x07\r\n",    request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/0.1\r\n", request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.4\r\n", request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/17.1\r\n",request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/2.1\r\n", request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/3.1\r\n", request::BadVersion, "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, badHeaderKey)
{
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\n" + std::string(65, 'x') + ": xyz\r\n", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\n: xyz\r\n",            request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nxyz\r\n:\r\n",         request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nhea der: xyz\r\n",     request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nh e a d e r: xyz\r\n", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nheader\x00: xyz\r\n",  request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\n\x00header: xyz\r\n",  request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\n\x00: xyz\r\n",        request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, badHeaderValue)
{
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nh:" + std::string(8193, 'v') + "\r\n", request::TooBigHeaders, "HTTP/1.1 431 Request Header Fields Too Large\r\nConnection: close\r\n\r\n");

    {
        std::string hdr = "h:" + std::string(4096, 'v') + "\r\n";
        SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\n" + hdr + hdr + hdr + hdr + hdr + hdr + hdr + hdr + "\r\n", request::TooBigHeaders, "HTTP/1.1 431 Request Header Fields Too Large\r\nConnection: close\r\n\r\n");
    }
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyUntilClose)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\n\r\n[this is a body]");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyUntilClose2)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nConnection:  close \r\n\r\n[this is a body]");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyByLength)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 16\r\n\r\n[this is a body]extra");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyChunked)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n10\r\n[this is a body]\r\n0\r\n\r\nextra");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyChunked2)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n"
                          "3\r\n[th\r\n"
                          "7\r\nis is a\r\n"
                          "6\r\n body]\r\n"
                          "0\r\n\r\n"
                          "extra");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
// echo -n "[this is a body]" | compress -c | hexdump -v -e '"\\" "x" 1/1 "%02X"'
// \x1F\x9D\x90\x5B\xE8\xA0\x49\x33\x07\x04\x41\x10\x61\x40\x88\x79\x43\x26\x4F\x17

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyCompress)
{
    // Content-Encoding: compress
    // не поддерживаем сразу

    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nTransfer-Encoding: compress\r\n\r\n"
                       "\x1F\x9D\x90\x5B\xE8\xA0\x49\x33\x07\x04\x41\x10\x61\x40\x88\x79\x43\x26\x4F\x17", request::UnprocessableContent, "HTTP/1.1 422 Unprocessable Content\r\nConnection: close\r\n\r\n");

    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: compress\r\n\r\n", request::UnprocessableContent, "HTTP/1.1 422 Unprocessable Content\r\nConnection: close\r\n\r\n");
}


/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
// echo -n "[this is a body]" | perl -MIO::Compress::RawDeflate -e 'undef $/; my ($in, $out) = (<>, undef); IO::Compress::RawDeflate::rawdeflate(\$in, \$out); print $out;' | hexdump -v -e '"\\" "x" 1/1 "%02X"'
// \x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDeflate)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nTransfer-Encoding: deflate\r\n\r\n"
                          "\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDeflate2)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Encoding: deflate\r\n\r\n"
                          "\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDeflate3)
{
    // extra
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: deflate\r\n\r\n"
                       "\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    // bad after middle
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: deflate\r\n\r\n"
                       "\x8B\x2E\xC9\xC8\x2C\x56 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
    // bad
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: deflate\r\n\r\n"
                       "abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
// echo -n "[this is a body]" | gzip -c - | hexdump -v -e '"\\" "x" 1/1 "%02X"'
// \x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00\xD0\x35\x3A\x02\x10\x00\x00\x00

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyGzip)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nTransfer-Encoding: gzip\r\n\r\n"
                          "\x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00\xD0\x35\x3A\x02\x10\x00\x00\x00");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyGzip2)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Encoding: gzip\r\n\r\n"
                          "\x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00\xD0\x35\x3A\x02\x10\x00\x00\x00");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyGzip3)
{
    // extra
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: gzip\r\n\r\n"
                       "\x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00\xD0\x35\x3A\x02\x10\x00\x00\x00 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    // bad after middle
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: gzip\r\n\r\n"
                       "\x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    // bad
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: gzip\r\n\r\n"
                       "abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
// echo -n "[this is a body]" | zstd -c - | hexdump -v -e '"\\" "x" 1/1 "%02X"'
// \x28\xB5\x2F\xFD\x00\x58\x81\x00\x00\x5B\x74\x68\x69\x73\x20\x69\x73\x20\x61\x20\x62\x6F\x64\x79\x5D

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyZstd)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nTransfer-Encoding: zstd\r\n\r\n"
                          "\x28\xB5\x2F\xFD\x00\x58\x81\x00\x00\x5B\x74\x68\x69\x73\x20\x69\x73\x20\x61\x20\x62\x6F\x64\x79\x5D");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyZstd2)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Encoding: zstd\r\n\r\n"
                          "\x28\xB5\x2F\xFD\x00\x58\x81\x00\x00\x5B\x74\x68\x69\x73\x20\x69\x73\x20\x61\x20\x62\x6F\x64\x79\x5D");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyZstd3)
{
    // extra
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: zstd\r\n\r\n"
                       "\x28\xB5\x2F\xFD\x00\x58\x81\x00\x00\x5B\x74\x68\x69\x73\x20\x69\x73\x20\x61\x20\x62\x6F\x64\x79\x5D abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    // bad after middle
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: zstd\r\n\r\n"
                       "\x28\xB5\x2F\xFD\x00\x58\x81\x00\x00\x5B\x74\x68\x69\x73\x20 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    // bad
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: zstd\r\n\r\n"
                       "abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
// echo -n "[this is a body]" | brotli -c - | hexdump -v -e '"\\" "x" 1/1 "%02X"'
// \x1B\x0F\x00\xF8\xA5\xB6\xBA\x52\x10\x45\x1A\x29\x17\x66\xDA\x29\x52

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyBrotli)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nTransfer-Encoding: br\r\n\r\n"
                          "\x1B\x0F\x00\xF8\xA5\xB6\xBA\x52\x10\x45\x1A\x29\x17\x66\xDA\x29\x52");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyBrotli2)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Encoding: br\r\n\r\n"
                          "\x1B\x0F\x00\xF8\xA5\xB6\xBA\x52\x10\x45\x1A\x29\x17\x66\xDA\x29\x52");
    spectacle.play();
    spectacle._peer->close();
    spectacle.play();

    CHECK_IO(spectacle);
    CHECK_INPUTDATA(spectacle, "[this is a body]");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyBrotli3)
{
    // extra
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: br\r\n\r\n"
                       "\x1B\x0F\x00\xF8\xA5\xB6\xBA\x52\x10\x45\x1A\x29\x17\x66\xDA\x29\x52 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    // bad after middle
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: br\r\n\r\n"
                       "\x1F\x0F\x00\xF8\xA5\xB6\xBA\x52 abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");

    // bad
    SERVER_PLAY_2_FAIL("GET uri HTTP/1.1\r\nContent-Encoding: br\r\n\r\n"
                       "abrakadabra shwabra", request::BadRequest, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, multiple)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 3\r\n\r\n012");
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 3\r\n\r\n012");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 5);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, out)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 1);
    www::http::server::Request input = spectacle._ios[0]._input;
    www::http::server::Response output = spectacle._ios[0]._output;

    output->firstLine(www::http::firstLine::Version::HTTP_1_1, 208, "DWESTI VOSEM");
    output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Content_Length, "3"}}, false);
    output->headers(List<www::http::Header>{www::http::Header{"xyz", "qwe"}}, true);
    output->data("012", true);
    output->done();
    spectacle.play();

    ASSERT_EQ(spectacle._peerDataMerged.toString(), "HTTP/1.1 208 DWESTI VOSEM\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, outMultiple)
{
    SpectacleForServer spectacle;
    spectacle._peer->send(
                "GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n"
                "GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n"
                "GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 3);
    auto responseOne = [&](std::size_t idx)
    {
        www::http::server::Request input = spectacle._ios[idx]._input;
        www::http::server::Response output = spectacle._ios[idx]._output;

        output->firstLine(www::http::firstLine::Version::HTTP_1_1, 208, "DWESTI VOSEM");
        output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Content_Length, "3"}}, false);
        output->headers(List<www::http::Header>{www::http::Header{"xyz", "qwe"}}, true);
        output->data("012", true);
        output->done();
    };
    responseOne(1);
    responseOne(2);
    responseOne(0);
    spectacle.play();

    ASSERT_EQ(spectacle._peerDataMerged.toString(),
              "HTTP/1.1 208 DWESTI VOSEM\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012"
              "HTTP/1.1 208 DWESTI VOSEM\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012"
              "HTTP/1.1 208 DWESTI VOSEM\r\nContent-Length: 3\r\nxyz: qwe\r\n\r\n012");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, outChunked)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 1);
    www::http::server::Request input = spectacle._ios[0]._input;
    www::http::server::Response output = spectacle._ios[0]._output;

    output->firstLine(www::http::firstLine::Version::HTTP_1_1, 200, "OK");
    output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Transfer_Encoding, "chunked"}}, true);
    output->data("[this is a body]", true);
    output->done();
    spectacle.play();

    ASSERT_EQ(spectacle._peerDataMerged.toString(),
              "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n"
              "10\r\n"
              "[this is a body]\r\n"
              "0\r\n\r\n");
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, outCompressed)
{
    SpectacleForServer spectacle;
    spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
    spectacle.play();

    CHECK_IO(spectacle);
    ASSERT_EQ(spectacle._ios.size(), 1);
    www::http::server::Request input = spectacle._ios[0]._input;
    www::http::server::Response output = spectacle._ios[0]._output;

    output->firstLine(www::http::firstLine::Version::HTTP_1_1, 200, "OK");
    output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Transfer_Encoding, "chunked, gzip"}}, true);
    output->data("[this is a body]", true);
    output->done();
    spectacle.play();

    ASSERT_EQ(spectacle._peerDataMerged.toString(),
              "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked, gzip\r\n\r\n"
              "22\r\n"
              "\x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00\xD0\x35\x3A\x02\x10\x00\x00\x00\r\n"
              "0\r\n\r\n"sv);
}

namespace
{
    void bodyDecompressGranuled(std::string_view compressor, std::string_view compressedText)
    {
        for(std::size_t granula{1}; granula<compressedText.size(); ++granula)
        {
            {
                SpectacleForServer spectacle;
                spectacle._peer->send("GET uri HTTP/1.1\r\n"
                                      "Transfer-Encoding: chunked\r\n"
                                      "Content-Encoding: "sv);
                spectacle._peer->send(compressor);
                spectacle._peer->send("\r\n\r\n"sv);
                spectacle.play();

                for(std::size_t pos{}; pos < compressedText.size(); pos += granula)
                {
                    std::string_view chunkBody = compressedText.substr(pos, granula);

                    std::array<char, 32> hexBuf;
                    std::string_view hex{hexBuf.data(), std::to_chars(hexBuf.data(), hexBuf.data()+hexBuf.size()-1, chunkBody.size(), 16).ptr};

                    spectacle._peer->send(hex);
                    spectacle._peer->send("\r\n"sv);
                    spectacle._peer->send(chunkBody);
                    spectacle._peer->send("\r\n"sv);
                    spectacle.play();
                }

                spectacle._peer->send("0\r\n\r\n"sv);

                spectacle._peer->close();
                spectacle.play();

                CHECK_IO(spectacle);
                CHECK_INPUTDATA(spectacle, "[this is a body]"sv);
            }

            {
                SpectacleForServer spectacle;
                spectacle._peer->send("GET uri HTTP/1.1\r\nContent-Length: 0\r\n\r\n");
                spectacle.play();

                CHECK_IO(spectacle);
                ASSERT_EQ(spectacle._ios.size(), 1);
                www::http::server::Request input = spectacle._ios[0]._input;
                www::http::server::Response output = spectacle._ios[0]._output;

                output->firstLine(www::http::firstLine::Version::HTTP_1_1, 200, "OK");
                output->headers(List<www::http::Header>{www::http::Header{www::http::header::KeyRecognized::Transfer_Encoding, std::string{compressor}}}, true);

                std::string_view decompressedText = "[this is a body]"sv;
                for(std::size_t pos{}; pos < decompressedText.size(); pos += granula)
                {
                    output->data(decompressedText.substr(pos, granula), false);
                    spectacle.play();
                }
                output->data(Bytes{}, true);
                spectacle.play();
                output->done();
                spectacle.play();

                ASSERT_FALSE(spectacle.has<SpectacleForServer::InputFailed>());
                ASSERT_FALSE(spectacle.has<SpectacleForServer::OutputFailed>());

                ASSERT_EQ(spectacle._peerDataMerged.toString(),
                          std::string{"HTTP/1.1 200 OK\r\nTransfer-Encoding: "} + std::string{compressor} + "\r\n\r\n" + std::string{compressedText});
            }
        }
    };
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDecompressGranuled_deflate)
{
    bodyDecompressGranuled("deflate", "\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00"sv);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDecompressGranuled_gzip)
{
    bodyDecompressGranuled("gzip", "\x1F\x8B\x08\x00\x00\x00\x00\x00\x00\x03\x8B\x2E\xC9\xC8\x2C\x56\x00\xA2\x44\x85\xA4\xFC\x94\xCA\x58\x00\xD0\x35\x3A\x02\x10\x00\x00\x00"sv);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDecompressGranuled_zstd)
{
    bodyDecompressGranuled("zstd", "\x28\xB5\x2F\xFD\x00\x58\x81\x00\x00\x5B\x74\x68\x69\x73\x20\x69\x73\x20\x61\x20\x62\x6F\x64\x79\x5D"sv);
}

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http_server, bodyDecompressGranuled_br)
{
    bodyDecompressGranuled("br", "\x1b\x0F\x00\xF8\xA5\xB6\xBA\x52\x10\x45\x1A\x29\x17\x66\xDA\x29\x52"sv);
}
