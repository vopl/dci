/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "request.hpp"
#include "response.hpp"
#include "channel.hpp"

namespace dci::module::www::http::server
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Request::Request()
        : Base{}
        , _response{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Request::~Request()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Request::setResponse(Response* response)
    {
        _response = response;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    io::InputProcessResult Request::process(bytes::Alter& data)
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
            _response = nullptr;
            reset();
            if(_api)
            {
                _api->done();
                _api->closed();
                _api.reset();
            }
            return io::InputProcessResult::done;

        case inputSlicer::Result::internalError:
            err4Fail = exception::buildInstance<api::http::error::request::InternalError>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::badEntity:
            err4Fail = exception::buildInstance<api::http::error::request::BadRequest>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::badMethod:
            err4Fail = exception::buildInstance<api::http::error::request::BadMethod>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::badVersion:
            err4Fail = exception::buildInstance<api::http::error::request::BadVersion>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::tooBigContent:
            err4Fail = exception::buildInstance<api::http::error::request::TooBigContent>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::tooBigUri:
            err4Fail = exception::buildInstance<api::http::error::request::TooBigUri>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::tooBigHeaders:
            err4Fail = exception::buildInstance<api::http::error::request::TooBigHeaders>(std::move(inputSlicerError));
            break;

        case inputSlicer::Result::unprocessableContent:
            err4Fail = exception::buildInstance<api::http::error::request::UnprocessableContent>(std::move(inputSlicerError));
            break;

        default:
            unreacheable();
            break;
        }

        _response->requestFailed(inputSlicerResult);
        _support->failed(this, std::move(err4Fail));

        return io::InputProcessResult::bad;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Request::sliceStart()
    {
        api::http::server::Request<> api;
        Base::setApi(api.init2());
        static_cast<Channel*>(_support)->emitIo(std::move(api));

        return inputSlicer::Result::done;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Request::sliceFlush(inputSlicer::state::RequestFirstLine& firstLine)
    {
        //std::cout << "[" << firstLine._method <<"][" << firstLine._uri << "][" << firstLine._version << "]" << std::endl;

        _api->firstLine(*firstLine._parsedMethod, std::move(firstLine._uri._downstream), *firstLine._parsedVersion);
        return IS::sliceFlush(firstLine);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Request::sliceFlush(inputSlicer::state::Headers& headers, bool done)
    {
        // if(header._empty)
        //     std::cout << "[" << header._key <<"][" << header._value << "]" << std::endl;
        // else
        //     std::cout << "[" << header._key <<"][" << header._value << "]" << std::endl;

        _api->headers(headers._conveyor.detachSome(), done);
        return IS::sliceFlush(headers, done);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inputSlicer::Result Request::sliceFlush(inputSlicer::state::Body& body, bool done)
    {
        // std::cout << "some body" << std::endl;

        _api->data(std::exchange(body._content, {}), done);
        return IS::sliceFlush(body, done);
    }
}
