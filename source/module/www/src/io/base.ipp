// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "base.hpp"

namespace dci::module::www::io
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    Base<Support, Impl, Api>::Base()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    Base<Support, Impl, Api>::Base(Support* support, Api&& api)
        : _support{support}
        , _api{std::move(api)}
    {
        setupApi();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    Base<Support, Impl, Api>::~Base()
    {
        _sol.flush();
        _api.reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    void Base<Support, Impl, Api>::setSupport(Support* support)
    {
        dbgAssert(!_support);
        dbgAssert(support);

        _support = support;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    void Base<Support, Impl, Api>::setApi(Api&& api)
    {
        dbgAssert(!_api);
        dbgAssert(api);

        _api = api;
        setupApi();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    void Base<Support, Impl, Api>::fireFailed(ExceptionPtr&& e)
    {
        if(_api)
            _api->failed(std::move(e));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    void Base<Support, Impl, Api>::fireClosed(bool andReset)
    {
        if(andReset)
            _sol.flush();

        if(_api)
        {
            _api->closed();

            if(andReset)
                _api.reset();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Support, class Impl, class Api>
    void Base<Support, Impl, Api>::setupApi()
    {
        dbgAssert(_api);

        // setupDataProcessing(tunable::Compression contentCompression, tunable::Compression transferCompression, tunable::Encoding transferEncoding)
        _api->setupDataProcessing() += _sol * [this](
            api::http::message::tunable::Compression contentCompression,
            api::http::message::tunable::Compression transferCompression,
            api::http::message::tunable::Encoding transferEncoding)
        {
            _contentCompression = contentCompression;
            _transferCompression = transferCompression;
            _transferEncoding = transferEncoding;
        };

        // in close();
        _api.methods()->close() += _sol * [this]()
        {
            _support->apiWantClose(static_cast<Impl*>(this));
        };
    }
}
