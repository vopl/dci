// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channel.hpp"

namespace dci::module::www::tls::server
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(SSL_CTX* sslCtx, const Settings& settings, api::stream::Channel<>&& peer)
        : api::tls::server::Channel<>::Opposite{idl::interface::Initializer{}}
        , tls::Channel{sslCtx, settings, *this, std::move(peer)}
    {
        SSL_set_accept_state(_ssl.get());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::Future<> Channel::handshake()
    {
        return tls::Channel::handshake();
    }
}
