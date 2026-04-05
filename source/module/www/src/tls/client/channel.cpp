/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
