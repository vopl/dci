// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    class Base
    {
    protected:
        Base();
        Base(Support* support, Api&& api);
        ~Base();

        void setSupport(Support* support);
        void setApi(Api&& api);

    public:
        void fireFailed(primitives::ExceptionPtr&&);
        void fireClosed(bool andReset = true);

    private:
        void setupApi();

    protected:
        Support*    _support{};
        Api         _api;
        api::http::message::tunable::Compression    _contentCompression{};
        api::http::message::tunable::Compression    _transferCompression{};
        api::http::message::tunable::Encoding       _transferEncoding{};
        sbs::Owner  _sol;
    };
}

#include "base.ipp"
