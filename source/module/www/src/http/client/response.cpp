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

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Response::Response(Support* support, api::http::client::Response<>::Opposite&& api)
        : Base{support, std::move(api)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Response::~Response()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    io::InputProcessResult Response::process(bytes::Alter& data)
    {
        inputSlicer::Result inputSlicerResult;
        ExceptionPtr inputSlicerError;
        {
            inputSlicer::SourceAdapter sa{data};
            std::tie(inputSlicerResult, inputSlicerError) = IS::process(sa);
        }

        ExceptionPtr err4Fail;
        switch(inputSlicerResult)
        {
        case inputSlicer::Result::needMore:
            dbgAssert(data.atBegin() && data.atEnd());
            return io::InputProcessResult::needMore;

        case inputSlicer::Result::done:
            reset();
            if(_api)
            {
                _api->done();
                _api.reset();
            }
            return io::InputProcessResult::done;

        case inputSlicer::Result::internalError:
            err4Fail = exception::buildInstance<api::http::error::response::InternalError>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::badEntity:
            err4Fail = exception::buildInstance<api::http::error::response::BadResponse>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::badVersion:
            err4Fail = exception::buildInstance<api::http::error::response::BadVersion>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::badStatus:
            err4Fail = exception::buildInstance<api::http::error::response::BadStatus>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::tooBigHeaders:
            err4Fail = exception::buildInstance<api::http::error::response::TooBigHeaders>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::tooBigContent:
            err4Fail = exception::buildInstance<api::http::error::response::TooBigContent>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::unprocessableContent:
            err4Fail = exception::buildInstance<api::http::error::response::UnprocessableContent>(std::move(inputSlicerError));
            break;

        default:
            unreacheable();
            break;
        }

        _support->failed(this, std::move(err4Fail));

        return io::InputProcessResult::bad;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Response::sliceFlush(inputSlicer::state::ResponseFirstLine& firstLine)
    {
        //std::cout << "[" << firstLine._method <<"][" << firstLine._uri << "][" << firstLine._version << "]" << std::endl;

        _api->firstLine(*firstLine._parsedVersion, firstLine._statusCode, std::move(firstLine._statusText._downstream));
        return IS::sliceFlush(firstLine);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Response::sliceFlush(inputSlicer::state::Headers& headers, bool done)
    {
        // if(header._empty)
        //     std::cout << "[" << header._key <<"][" << header._value << "]" << std::endl;
        // else
        //     std::cout << "[" << header._key <<"][" << header._value << "]" << std::endl;

        _api->headers(headers._conveyor.detachSome(), done);
        return IS::sliceFlush(headers, done);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Response::sliceFlush(inputSlicer::state::Body& body, bool done)
    {
        // std::cout << "some body" << std::endl;

        _api->data(std::exchange(body._content, {}), done);
        return IS::sliceFlush(body, done);
    }
}
