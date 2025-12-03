// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "io/plexus.hpp"
#include "../outputBase.hpp"

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Response;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Request
        : public OutputBase<io::Plexus<Response, Request, false>, Request, api::http::client::Request<>::Opposite>
        , public mm::heap::Allocable<Request>
    {
        using Support = io::Plexus<Response, Request, false>;
        using Base = OutputBase<io::Plexus<Response, Request, false>, Request, api::http::client::Request<>::Opposite>;

    public:
        Request(Support* support, api::http::client::Request<>::Opposite&& api);
        ~Request();

    private:
        friend Base;
        static ExceptionPtr makeExceptionAmbiguousHeaders(ExceptionPtr cause = {});
        static ExceptionPtr makeExceptionCompressionFailed(ExceptionPtr cause = {});
    };
}
