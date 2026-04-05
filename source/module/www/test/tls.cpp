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
// #include <dci/cmt.hpp>
#include "www4test.hpp"

using namespace dci;
using namespace dci::host;
// using namespace dci::cmt;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_tls, DISABLED_probe)
{
    www::Tls<> api = *testManager()->createService<www::Tls<>>();
    net::Host<> netHost = *testManager()->createService<net::Host<>>();
    net::stream::Channel<> netPeer = netHost->streamClient().value()->connect(net::Ip4Endpoint{net::Ip4Address{127,0,0,1}, 443}).value();
    www::stream::Channel<>::Opposite peer{interface::Initializer{}};

    sbs::Owner sol;

    peer->send()        += sol * [netPeer](Bytes&& data) { return netPeer->send(std::move(data)); };
    peer->startReceive()+= sol * [netPeer]()             { return netPeer->startReceive();        };
    peer->stopReceive() += sol * [netPeer]()             { return netPeer->stopReceive();         };
    peer->shutdown()    += sol * [netPeer]()             { return netPeer->shutdown(true, true);  };
    peer->close()       += sol * [netPeer]()             { return netPeer->close();               };

    netPeer->received() += sol * [peer](Bytes&& data)         { peer->received(std::move(data)); };
    netPeer->failed()   += sol * [peer](ExceptionPtr&& err)   { peer->failed(std::move(err));    };
    netPeer->closed()   += sol * [peer]()                     { peer->closed();                  };

    www::tls::client::Channel<> tlsClient;
    try
    {
        api->setAlpnProtos(List<String>{/*"h2",*/ "http/1.1", "http/1.0", "http/0.9"}).value();

        api->setServername("example.com");

        api->setTmpDh("-----BEGIN DH PARAMETERS-----\n"
                      "MIICDAKCAgEAgxR3P6SC7YifDYPsq/gvd1iZJKPfxJpR/D8/cBH45gzjttwXJqiL\n"
                      "VX4vlnbeCwg1nDy64CRQo3EwOMMhquDGkojtCwFNofJ8gtIB5pmgv4oQRAstgRPR\n"
                      "2SXfWyuBde4cHcePFhwC/6GpxHXePan/5LCDeniA5+2gTVkE64oFbr5dCfW/Gsvm\n"
                      "F56idw55eEzoq5MdQkB0p2q6xeKy0ba4G+Gcw9PE5a5NwbgJyuCSHt9TJYBU4N0p\n"
                      "15Ng+6UcAnCt+GJ1kltjhSvqpID4YfffHmTdDJpaAnafmaFzykpd9hAnz67mMtvJ\n"
                      "M+SFJpagQWSa9Hjyprv46qtGG2v5fGpRRiE3fRpBpx29L6vFyFKZbstTd/00wK45\n"
                      "KWBvI0v9gTdI2Lc0wXrTIq538IohqIy2XRMeGtnD74i252/qUqBx7hDZQHecAcqN\n"
                      "cnlXK8DyZX69aHD6A7gXA/Ich7Qb2hrkk/bk+HSux6/vLPum5j4sCUVh0N+2Eo4c\n"
                      "rVdJgjSaSS4hTSgAzDY16HKb7XBwCfPuB0DrSUpVNRZ2Hy3mgQAsoVcIM9byi8Ys\n"
                      "jKH9b4caYFYsWmNImtaAyFN7+ynDVFJO4uCcMwi+SHy3/uYQTdB4TPotsS/oB40i\n"
                      "z6fmeAy0OJa53VZWHuCv9ZfoBYd87tgTc91sY87tK9G2RtEuO2ZsrEsCAQUCAgFF\n"
                      "-----END DH PARAMETERS-----").value();

        std::string ca = R"(-----BEGIN CERTIFICATE-----
MIIFbTCCA1WgAwIBAgIUPJ2bikn57dXnnCvSfQPdTFxTrDMwDQYJKoZIhvcNAQEL
BQAwRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoM
GEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDAgFw0yNDEyMDMxNjA2NDZaGA8yMTI0
MTEwOTE2MDY0NlowRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUx
ITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCAiIwDQYJKoZIhvcN
AQEBBQADggIPADCCAgoCggIBANU4SgeEnswkA75MB5B3/CxKJNT8D8eQfd8JI+3e
/ocHQ7FlKJliVwIyy9fr+/en6H1+gZaSSzHm81CoXXk+/5BY+MZD+RTIaUW/225b
5XYPOP2FWS4HO+rwsJoQMds4CBOjkyqhT8XTWaA0dUDrmt2kHdQi1k9nwmrOyCGg
Mjb4zd36ZrBJtnxVfGxNuNPL13rC4txB70mA0faJg8N9YMHharldtIv87M6wTRrh
GcYl5utUafejZ85fBNb0scARD9bMsIGyVvFMM65/oRPuMFo0K1toj02BL9lEoqCo
AA+dN9DA4fSFhVo9iME27KSBjJTtJwYR0zJazYeBZOQw9UM3LChJ/LON4eAOlLp2
rGBkKLvRGkvB0tfS0G9J9kIfMOYjYO8pVfzOCX6w1DMW2RmsBVgDwmnmLg+Et2GE
WOw31rRfdJT1t6VimTysCX859ED1LMTbcvbYN6WK5XBqs/MIZ70lYl8qJU9q1ZQ+
D1TYMDRMNrU823G+NuJsJ9S3DlNz3s5YUy4R4F9IWmjvaSyb6eJTHGGvn5p77cus
tA45rdTXQvYJAvQI/CKZzqLaO3Oes1GEi17Qi13xQiIDU1rT4lPxx/NHr4pQrGdx
VWxlmB7rN5cf73JvXqGmza2ieZmdFniANss7nN/gopljA1JV4hKhhNRvLbYrpAOF
6r/pAgMBAAGjUzBRMB0GA1UdDgQWBBR8pEnbOs3V2qWYFetW+iyBgzjMijAfBgNV
HSMEGDAWgBR8pEnbOs3V2qWYFetW+iyBgzjMijAPBgNVHRMBAf8EBTADAQH/MA0G
CSqGSIb3DQEBCwUAA4ICAQCJ+rfoGKYYXlAvAj/WyOLTSJfG3Mu3KY3yw4CQGo5g
uSEJ8PiUkx9tX1BfLgihU7k0jZRAtDnxIwmgq9nGeZq0IDHwNzoKjB/yWN9qgUVj
A0Ipp4CULMROJWPa/NzWlhOSz6vZzb2Qbl4NBqYioL48qrToTGhHI5jV++vutxr7
Z4Gup9bFp7TCHRVp1k2cUApYRNpsIhcNemmioUCNlgMAuLx/vVsgA+qs9YPV3rBy
HpNKYmNoi0h0lvsWiBxkzfwGLkmnRjghOqYoNakhX7S3eHGi/shoexYTwStuarIW
lKv3HTVREi3kt5Km2hzU5Apv5PGmTBsjLOwaA3WJflYeg7+00BoTOjfx5339S8Av
Fxf3F4/vumyXc/UJWg5H7hMdTGDT3rwnzvdbE+KS3/c07BB8oa1w4EIbzXDDW4yB
hDzp82faqbY6UYpvxNIk2FKYuhZFdPRjRDEwmms9zPIXPsyf6FtPwOsDp0Gr1Btq
OB4BvMChcV92VwpLnD13lvIE2wvYT+/YSeeYkFSnuVPniaOj9GrF0hnLfxkci3Fj
KXT7KtqShzcrgQkHQpH6Gcsro7i4PJiPqF5odsFtIhrtaaOHtNY2b6njNOwmrD53
5dMxOEryyYHDTbNd7EuKUaaEI00bJQUFeBnh4qVVsN9SGZaOD8C8ykFaioEzh+n2
gg==
-----END CERTIFICATE-----)";

        std::string cert = R"(-----BEGIN CERTIFICATE-----
MIIFVTCCAz2gAwIBAgIEdCg3RjANBgkqhkiG9w0BAQsFADBFMQswCQYDVQQGEwJB
VTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0
cyBQdHkgTHRkMB4XDTI0MTIwMzE3MDYzNloXDTI1MTIwMzE3MDYzNlowRTELMAkG
A1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGEludGVybmV0
IFdpZGdpdHMgUHR5IEx0ZDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIB
AKRm/MIYVb9L3g4V5K7ru8dAAoIrg9QMJKahnP56jNQxb0GkatveJ1BPPpf8+5+r
rYMq7TTvq18H0DLfNsK5yYlIofyI1lxztT+XbVL14c4s6eSKxuwV0NLjZ9PbN48x
QdcwzvXIV6SC1WBtBmEhAYrCBttiUdyUGinJX09xcltYONswkaj8fT/Ytm7a8s//
JBsPfAZxvBr0/gFScJlveu7obnjc6/kA3HylIxKvus0g9qbyDgpCM2Pe2Gc/tgPg
uH9hZcTvA5/vdLKlbrQiNkzp1VZnVaS8+a7EE48P+YuJOHI4n4dzaLKYxsz6/w5J
bBuNDyxrZVb4j7Tv04YLjAHvcXQixQn1r5skk4LU9fDcm134hiqJ4pJJh5vKvOir
aFoSgokg8vR4sYGwfRUmIK3yv4NxVRiPmPWjzN2Hu6HbM/rGL66Bu+HJHRhjrNdx
ust65er00t8Bv8m3fhNGkANnK9mwNii+LOeWOH975jIv8nqZxUxS7l0EJqsWFi30
BekOlxbmpWt5KxR4aLgOBaLjHnv3buL4By2E79n2BcGZpFo6RVy9OfvhJ4bFzHId
94DqKZwzFmD9Pd5SKszZPw7VxEDyDP9pGs1YsmlW6IfHot3AJPucjKT/SnYAAdpV
bvaCSXkA8Wo//OWd0QEso7WO2MxKYAMVhwSlqzfKolHJAgMBAAGjTTBLMAkGA1Ud
EwQCMAAwHQYDVR0OBBYEFN1JHcfT1jy0TGbKe7WCEE14wb11MB8GA1UdIwQYMBaA
FHykSds6zdXapZgV61b6LIGDOMyKMA0GCSqGSIb3DQEBCwUAA4ICAQBBObCzGUdH
/yWIUAHoqU4+RwKl2J/ZaJbHYcRzVNsIDCZyCaplH9PkkSaFkuOolI1YhE4a0yJg
mMa7S1muqg7GPX0b1Pp1UXC1p/tFRYlMqkVlf9Wh1cHADPPht4//J75olYvNvH78
mCVVBdg+J+N91JtrXCwuTNUzX1dCMkwQEotU8Xlas2sFKqCJQyV28swnSzluvhGo
oOOp6vTW3YpOOkVGGRZ1ZlcjBTwnFCwY25T8QHU55UFAiKU6KlswRg8F9V4DW7ph
9k25gkbrGyVw+8c5QNE17TdHHcImqHCUK6jBNoAHo1JtGA6Cm6XtDTkzsVZd7TgR
vz/GN++k49m0ZffWsdOSyNbNAVQein98GcQekV0Ed+paoAnHcw2jfnF8dDTdxeAx
vtGs1ZTQKTtqHCWxq0E3fDXM27GWYpG6nhgGWig6RAqsKbvkVAOqkvaX6OmFRWud
LBktKzyqHnevzC8j6Wwm4gB4xIq6TKEME9EvgsFtSpWFHv4ibGN/VBdyavave+Hh
/VVjjQuYpFlBYaVfbEdc3MXvfYqSF32kHfyp3WSoM9aR42sNXbD69VX5vWDc8MGK
NfwNhCFDzQa2073wHe3AAqnU1WvIvupL878Ts1vBzXgJauExqM/qq7QjQq2rnxcg
/I7GEREdZNzzqDojQbznLd7ICOjb0N4+Xg==
-----END CERTIFICATE-----)";

        std::string key = R"(-----BEGIN ENCRYPTED PRIVATE KEY-----
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
I1a5qVepG+qa1fYeyOfkgRdS4yICIa2f
-----END ENCRYPTED PRIVATE KEY-----)";

        std::string srvCert = R"(-----BEGIN CERTIFICATE-----
MIIFyjCCA7KgAwIBAgIBAjANBgkqhkiG9w0BAQsFADCBqTELMAkGA1UEBhMCVVMx
EzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDVNhbnRhIEJhcmJhcmExEzAR
BgNVBAoMClNTTCBTZXJ2ZXIxIjAgBgNVBAsMGUZvciBUZXN0aW5nIFB1cnBvc2Vz
IE9ubHkxFTATBgNVBAMMDGxvY2FsaG9zdCBDQTEdMBsGCSqGSIb3DQEJARYOcm9v
dEBsb2NhbGhvc3QwHhcNMjAxMjMxMTE1NjQzWhcNMjIxMjMxMTE1NjQzWjCBpjEL
MAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDVNhbnRh
IEJhcmJhcmExEzARBgNVBAoMClNTTCBTZXJ2ZXIxIjAgBgNVBAsMGUZvciBUZXN0
aW5nIFB1cnBvc2VzIE9ubHkxEjAQBgNVBAMMCWxvY2FsaG9zdDEdMBsGCSqGSIb3
DQEJARYOcm9vdEBsb2NhbGhvc3QwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
AoICAQCtg54l4Ox/+psUlHEtydveFLPOGtlRavHaBfB8X/D4a3M1tDvMEKdhS7vz
WeRV3G34RCiUkalXmNiF/OJL3rETNa492O98GuEQfCZhnBA67/kWQg5xdmmaWfE9
toDE18l77AQ1B4X+RjzXBIfelDhZ+zMOsIudj9W4iOnyn2AgCtocSVV6yCGYvJRq
oLcYvbKq8dOBul0xZTPRvFuTnruSLreyhUbVDadIaqDXGJDQeIa53IkBFqvTIQ4f
EEsQo5uqcjH1iI8OlAVe77p0KNzr6xj1haCniS7dgppi4kKxTiYtcpMZxdLvbJyw
ZQbo9MLktDxbL0+Xsq680t0jdNlkH0Dy8j9HUPi/FwUMszfw/L3RXhnPcgOXIRJN
8U2lhnbMHcS9Xnj+oeBEydnAOp5NLJTQH/bmhcrF+fILGVdjX5nGM30fgGK0ElB7
a6q1N7x5/PZubWGbS5PMFDJIqs+Ery6eW5B/shZptc4ZuUnM5VYK143ktpK+TVpo
a5cpWDkYmZGVn9oBmMZlILm9EojC8NDKzZuNtJ04qgBOAtXUzwNFTCVCX2NgSk/b
0WLJjZ+9vWrdKskI4PPxdmY9aXe1s1Le0zYQbBxuZXtvmfTSSPKq5dg2xphTnJO6
iYM+8/0miS+xgswX6nKbb62b2uRufhIUmpiZh1esSUNW4Im8XwIDAQABMA0GCSqG
SIb3DQEBCwUAA4ICAQCTkr2d+sW/rDRLkMdpMPy267FLdEAcPCCcEtyd1PtrPpKb
xsSmq4WMc9UiM0ujxGmweRdUrsf9uiBNa96UO6WEK3L1LiQtFIRjWPKQE4f6WU3H
oo1m8sjBzlFD/mwrrqRYdNRu7VCW4wb+r1OJbuNKw/tzWLhpfsNIVo3dJiTO6l65
zxfswQMWEL2C5yl3uzt6MXBU0G0OHvWSutGzj8cgIOuDYg0QBC5rdTbS/ZcQx6kd
NWfX1qynS6t1uh/d66xNyFGW4g0uEcnhrnz78uhP2683KrMPKSH5OoOj7boKe4b3
s1s7bCgzqTQLYrPR3YAksSvsUiSivhC1l5ImRWlK+M8xeKXXmW/xh7x3PbeCLM00
AJu+zwVSrH8dhMZUQGkdx6Im9J6X8fg6DBwf0bVA+AOjPeYXyRrcbPWTe5MVMLEc
PZRP6b0Bkca6UgTHggg5CQ0JLwbNZxnTks2Vq5SsfzLUz1mBuh3PZ2HgRjv7axOb
Ks0FGnBV814OkcGy1h9xx/sGPDtWs6wfl34C+a3pkLSFYU/2ue5x7RJS29+Qigrb
iGCUkL6ezBFOjH6ujwF7vUaGawy20iTMC+lPj5sYGxZBXuKBLLZ9outSuGQ3AGeX
dR0p5QAGVGfLcjGdHNEtkpsdVgOmEgyMChzLfxNpWRgRTljlq1vYWZDUJx/aLQ==
-----END CERTIFICATE-----
)";
        api->setAuth(cert + "\n" + ca, "", key, "123456").value();

        api->setDefaultTrustedCAs().value();
        api->addTrustedCAs(List<String>{srvCert}).value();
        api->setVerify(true, 100).value();

        tlsClient = api->client(peer, std::nullopt).value();
    }
    catch(...)
    {
        LOGD(dci::exception::toString(std::current_exception()));
        return;
    }

    LOGD("alpnProtoSelected: " << tlsClient->alpnProtoSelected().value());

    tlsClient->startReceive();
    tlsClient->failed() += sol * [](ExceptionPtr&& error)
    {
        LOGD("tls error: " << (error ? dci::exception::toString(error) : std::string{}));
    };
    tlsClient->closed() += sol * []()
    {
        LOGD("tls closed");
    };
    tlsClient->received() += sol * [](Bytes&& data)
    {
        LOGD("some received: [" << data.toString() << "]");
    };

    tlsClient->send("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
    poll::timeout(std::chrono::milliseconds{250}).wait();

    tlsClient->send("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
    poll::timeout(std::chrono::milliseconds{250}).wait();

    sol.flush();
}
