// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
