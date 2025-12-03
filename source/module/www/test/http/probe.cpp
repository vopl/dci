// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/test.hpp>
#include <dci/host.hpp>
#include <dci/poll.hpp>
#include <dci/cmt.hpp>
#include "www.hpp"
#include "utils.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::cmt;
using namespace dci::idl;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
TEST(module_www_http, probe)
{
    auto [cln, srv] = http::utils::interconnectWwwHttp();

    sbs::Owner sol;
    cmt::Event srvFirstLineDone, srvHeadersDone, srvDataDone;

    primitives::List<www::http::Header> srvHeaders;
    Bytes srvData;

    // server request
    srv->io() += sol * [&](www::http::server::Request<> srvReq, www::http::server::Response<> /*srvResp*/)
    {
        srvReq->firstLine() += sol * [&](www::http::firstLine::Method method, dci::primitives::String&& path, www::http::firstLine::Version version)
        {
            EXPECT_EQ(method, www::http::firstLine::Method::GET);
            EXPECT_EQ(path, "/");
            EXPECT_EQ(version, www::http::firstLine::Version::HTTP_1_1);
            srvFirstLineDone.raise();
        };

        srvReq->headers() += sol * [&](primitives::List<www::http::Header>&& headers, bool done)
        {
            srvHeaders.insert(srvHeaders.end(), headers.begin(), headers.end());
            if(!done)
                return;

            ASSERT_EQ(2, srvHeaders.size());

            EXPECT_EQ(srvHeaders[0].key, www::http::header::KeyRecognized::Host);
            EXPECT_EQ(srvHeaders[0].value, "localhost");

            EXPECT_EQ(srvHeaders[1].key, "x-my-header");
            EXPECT_EQ(srvHeaders[1].value, "x-my-value");

            srvHeadersDone.raise();
        };

        srvReq->data() += sol * [&](Bytes&& data, bool done)
        {
            srvData.end().write(std::move(data));
            if(!done)
                return;

            EXPECT_EQ(srvData, Bytes{"xyz"});
            EXPECT_TRUE(done);
            srvDataDone.raise();
        };
    };

    // client request
    {
        www::http::client::Request<> clnReq;
        www::http::client::Response<> clnResp;
        cln->io(clnReq.init2(), clnResp.init2());

        clnReq->firstLine(www::http::firstLine::Method::GET, "/", www::http::firstLine::Version::HTTP_1_1);
        clnReq->headers(
                    primitives::List<www::http::Header> {
                        {www::http::header::KeyRecognized::Host, "localhost"},
                        {"x-my-header", "x-my-value"},
                    }, true);
        clnReq->data("xyz", true);
        clnReq->done();
        cln.reset();
    }

    auto wres = cmt::wait(poll::timeout(std::chrono::seconds{1}) || (srvFirstLineDone && srvHeadersDone && srvDataDone));
    EXPECT_EQ(wres.to_string(), "0111");
    sol.flush();
}
