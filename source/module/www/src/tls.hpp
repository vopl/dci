// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "tls/channel.hpp"

namespace dci::module::www
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Tls
        : public api::Tls<>::Opposite
        , public host::module::ServiceBase<Tls>
    {
    public:
        Tls();
        ~Tls();

    private:
        template <class Api, class Impl>
        cmt::Future<Api> launch(api::stream::Channel<>&& stream, String* servername = nullptr);

    private:
        std::unique_ptr<SSL_CTX, void(*)(SSL_CTX*)> _sslCtx;
        tls::Channel::Settings _settings;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Api, class Impl>
    cmt::Future<Api> Tls::launch(api::stream::Channel<>&& stream, String* servername)
    {
        Impl* impl;
        if(servername)
        {
            tls::Channel::Settings settings{_settings._encodedAlpnProtos, std::move(*servername)};
            impl = new Impl{_sslCtx.get(), settings, std::move(stream)};
        }
        else
            impl = new Impl{_sslCtx.get(), _settings, std::move(stream)};

        Api api = *impl;
        impl->involvedChanged() += impl->sol() * [impl](bool v)
        {
            if(!v)
                delete impl;
        };

        return impl->handshake().chain() += impl->sol() * [api = std::move(api)](cmt::Future<>& in, cmt::Promise<Api>& out) mutable
        {
            if(in.resolvedCancel())
                out.resolveCancel();
            else if(in.resolvedException())
                out.resolveException(in.detachException());
            else if(in.resolvedValue())
                out.resolveValue(std::move(api));
            api.reset();
        };
    }
}
