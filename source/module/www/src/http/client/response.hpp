/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "io/plexus.hpp"
#include "io/inputBase.hpp"
#include "../inputSlicer.hpp"

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Request;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Response
        : public io::InputBase<io::Plexus<Response, Request, false>, Response, api::http::client::Response<>::Opposite, false>
        , protected InputSlicer<inputSlicer::Mode::response, Response>
        , public mm::heap::Allocable<Response>
    {
        using Support = io::Plexus<Response, Request, false>;
        using Base = io::InputBase<io::Plexus<Response, Request, false>, Response, api::http::client::Response<>::Opposite, false>;
        using IS = InputSlicer<inputSlicer::Mode::response, Response>;

    public:
        Response() = delete;
        Response(Response&&) = delete;
        Response(const Response&) = delete;
        Response(Support* support, api::http::client::Response<>::Opposite&& api);
        ~Response();

        io::InputProcessResult process(bytes::Alter& data);

    private:
        friend IS;
        inputSlicer::Result sliceFlush(inputSlicer::state::ResponseFirstLine& firstLine);
        inputSlicer::Result sliceFlush(inputSlicer::state::Headers& headers, bool done);
        inputSlicer::Result sliceFlush(inputSlicer::state::Body& body, bool done);
    };
}
