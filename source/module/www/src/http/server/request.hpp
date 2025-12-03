// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "io/plexus.hpp"
#include "io/inputBase.hpp"
#include "../inputSlicer.hpp"

namespace dci::module::www::http::server
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Response;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Request
        : public io::InputBase<io::Plexus<Request, Response, true>, Request, api::http::server::Request<>::Opposite, true>
        , protected InputSlicer<inputSlicer::Mode::request, Request>
        , public mm::heap::Allocable<Request>
    {
        using Support = io::Plexus<Request, Response, true>;
        using Base = io::InputBase<io::Plexus<Request, Response, true>, Request, api::http::server::Request<>::Opposite, true>;
        using IS = InputSlicer<inputSlicer::Mode::request, Request>;

    public:
        Request();
        ~Request();

        void setResponse(Response* response);
        io::InputProcessResult process(bytes::Alter& data);

    private:
        friend IS;
        inputSlicer::Result sliceStart();
        inputSlicer::Result sliceFlush(inputSlicer::state::RequestFirstLine& firstLine);
        inputSlicer::Result sliceFlush(inputSlicer::state::Headers& headers, bool done);
        inputSlicer::Result sliceFlush(inputSlicer::state::Body& body, bool done);

    private:
        Response* _response{};
    };
}
