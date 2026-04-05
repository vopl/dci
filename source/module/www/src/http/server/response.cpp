/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "response.hpp"
#include "request.hpp"
#include "../../enumSupport.hpp"

namespace dci::module::www::http::server
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Response::Response(Support* support, api::http::server::Response<>::Opposite&& api)
        : Base{support, std::move(api)}
    {
        // in firstLine(firstLine::Method, string path, firstLine::Version);
        _api.methods()->firstLine() += _sol * [this](api::http::firstLine::Version version, api::http::firstLine::StatusCode statusCode, primitives::String&& statusText)
        {
            bytes::Alter out{_buffer.end()};

            std::optional<std::string_view> optStr = enumSupport::toString(version);
            if(!optStr)
            {
                _support->failed(this, exception::buildInstance<api::http::error::response::BadVersion>());
                _support->close({});
                return;
            }
            out.write(optStr->data(), optStr->size());

            out.write(" ");
            char statusCodeBuf[16];
            std::to_chars_result scres = std::to_chars(std::begin(statusCodeBuf), std::end(statusCodeBuf), statusCode);
            out.write(&statusCodeBuf[0], scres.ptr - &statusCodeBuf[0]);
            out.write(" ");
            out.write(statusText.data(), statusText.size());
            out.write("\r\n");
        };

        // OutputBase implements:
        //      in headers(list<Header>, bool done);
        //      in data(bytes, bool done);
        //      in done();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Response::~Response()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Response::requestFailed(inputSlicer::Result inputSlicerResult)
    {
        if(_someWote)
            return;

        switch(inputSlicerResult)
        {
        default:
        case inputSlicer::Result::badStatus:
            dbgAssert("bad inputSlicerResult value");
            [[fallthrough]];
        case inputSlicer::Result::internalError:
        case inputSlicer::Result::badEntity:
            _buffer = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n";
            break;
        case inputSlicer::Result::badMethod:
            _buffer = "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n";
            break;
        case inputSlicer::Result::badVersion:
            _buffer = "HTTP/1.1 505 HTTP Version Not Supported\r\nConnection: close\r\n\r\n";
            break;
        case inputSlicer::Result::tooBigContent:
            _buffer = "HTTP/1.1 413 Content Too Large\r\nConnection: close\r\n\r\n";
            break;
        case inputSlicer::Result::tooBigUri:
            _buffer = "HTTP/1.1 414 URI Too Long\r\nConnection: close\r\n\r\n";
            break;
        case inputSlicer::Result::tooBigHeaders:
            _buffer = "HTTP/1.1 431 Request Header Fields Too Large\r\nConnection: close\r\n\r\n";
            break;
        case inputSlicer::Result::unprocessableContent:
            _buffer = "HTTP/1.1 422 Unprocessable Content\r\nConnection: close\r\n\r\n";
            break;
        }

        flushBuffer();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Response::someWrote()
    {
        _someWote = true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionPtr Response::makeExceptionAmbiguousHeaders(ExceptionPtr cause)
    {
        return exception::buildInstance<api::http::error::response::AmbiguousHeaders>(std::move(cause));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ExceptionPtr Response::makeExceptionCompressionFailed(ExceptionPtr cause)
    {
        return exception::buildInstance<api::http::error::response::CompressionFailed>(std::move(cause));
    }
}
