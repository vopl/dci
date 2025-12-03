// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "io/plexus.hpp"
#include "../outputBase.hpp"
#include "../inputSlicer/result.hpp"

namespace dci::module::www::http::server
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Request;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Response
        : public OutputBase<io::Plexus<Request, Response, true>, Response, api::http::server::Response<>::Opposite>
        , public mm::heap::Allocable<Response>
    {
        using Support = io::Plexus<Request, Response, true>;
        using Base = OutputBase<io::Plexus<Request, Response, true>, Response, api::http::server::Response<>::Opposite>;

    public:
        Response(Support* support, api::http::server::Response<>::Opposite&& api);
        ~Response();

        void requestFailed(inputSlicer::Result inputSlicerResult);

    private:
        friend Base;
        void someWrote();
        static ExceptionPtr makeExceptionAmbiguousHeaders(ExceptionPtr cause = {});
        static ExceptionPtr makeExceptionCompressionFailed(ExceptionPtr cause = {});

    private:
        bool _someWote{};
    };
}
