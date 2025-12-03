// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "request.hpp"
#include "response.hpp"
#include "../../enumSupport.hpp"
#include "../../utils.hpp"

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Request::Request(Support* support, api::http::client::Request<>::Opposite&& api)
        : Base{support, std::move(api)}
    {
        // in firstLine(firstLine::Method, string path, firstLine::Version);
        _api.methods()->firstLine() += _sol * [this](api::http::firstLine::Method method, primitives::String&& path, api::http::firstLine::Version version)
        {
            bytes::Alter out{_buffer.end()};

            std::optional<std::string_view> optStr = enumSupport::toString(method);
            if(!optStr)
            {
                _support->failed(this, exception::buildInstance<api::http::error::request::BadMethod>());
                _support->close({});
                return;
            }
            out.write(optStr->data(), optStr->size());

            out.write(" ");
            out.write(path.data(), path.size());
            out.write(" ");

            optStr = enumSupport::toString(version);
            if(!optStr)
            {
                _support->failed(this, exception::buildInstance<api::http::error::request::BadVersion>());
                _support->close({});
                return;
            }
            out.write(optStr->data(), optStr->size());
            out.write("\r\n");
        };

        // OutputBase implements:
        //      in headers(list<Header>, bool done);
        //      in data(bytes, bool done);
        //      in done();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Request::~Request()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionPtr Request::makeExceptionAmbiguousHeaders(ExceptionPtr cause)
    {
        return exception::buildInstance<api::http::error::request::AmbiguousHeaders>(cause);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionPtr Request::makeExceptionCompressionFailed(ExceptionPtr cause)
    {
        return exception::buildInstance<api::http::error::request::CompressionFailed>(cause);
    }
}
