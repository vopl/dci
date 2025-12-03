// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channel.hpp"
#include "../utils.hpp"

namespace dci::module::www::tls::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(SSL_CTX* sslCtx, const Settings& settings, api::stream::Channel<>&& peer)
        : api::tls::client::Channel<>::Opposite{idl::interface::Initializer{}}
        , tls::Channel{sslCtx, settings, *this, std::move(peer)}
    {
        SSL_set_connect_state(_ssl.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Channel::handshake()
    {
        ERR_clear_error();
        int opResult = SSL_set_alpn_protos(
                            _ssl.get(),
                            _settings._encodedAlpnProtos.data(),
                            _settings._encodedAlpnProtos.size());
        if(0 != opResult)
            return cmt::readyFuture<>(exception::buildInstance<api::tls::error::SetAlpnProtosFailed>());

        if(!_settings._servername.empty())
        {
            opResult = SSL_set_tlsext_host_name(_ssl.get(), _settings._servername.c_str());
            if(1 != opResult)
            {
                unsigned long error = ERR_get_error();
                return cmt::readyFuture<>(exception::buildInstance<api::tls::error::SetServernameFailed>(tls::utils::errorString(error)));
            }

            opResult = SSL_set1_host(_ssl.get(), _settings._servername.c_str());
            if(1 != opResult)
            {
                unsigned long error = ERR_get_error();
                return cmt::readyFuture<>(exception::buildInstance<api::tls::error::SetServernameFailed>(tls::utils::errorString(error)));
            }
        }

        return tls::Channel::handshake();
    }
}
