/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "factory.hpp"
#include "tls.hpp"
#include "http/client/channel.hpp"
#include "http/client/cookies.hpp"
#include "http/server/channel.hpp"
#include "http2/client/channel.hpp"
#include "http2/server/channel.hpp"
#include "ws/channel.hpp"
#include "agent.hpp"

namespace dci::module::www
{
    namespace
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template <class Impl>
        auto createImpl(auto&&... args)
        {
            Impl* impl = new Impl{std::forward<decltype(args)>(args)...};
            auto fetchSol = [&]()->sbs::Owner&
            {
                if constexpr(requires{impl->serviceSol();})
                    return impl->serviceSol();
                else
                    return impl->sol();
            };
            impl->involvedChanged() += fetchSol() * [impl](bool v)
            {
                if(!v)
                    delete impl;
            };

            return impl->opposite();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Factory::Factory(host::Manager* hostManager)
        : api::Factory<>::Opposite{idl::interface::Initializer{}}
        , _hostManager{hostManager}
    {
        // in tls() -> Tls;
        methods()->tls() += serviceSol() * []()
        {
            return cmt::readyFuture(createImpl<Tls>());
        };

        // in stream2HttpClient(stream::Channel) -> http::client::Channel;
        methods()->stream2HttpClient() += serviceSol() * [](api::stream::Channel<>&& streamChannel)
        {
            return cmt::readyFuture(createImpl<http::client::Channel>(std::move(streamChannel)));
        };

        // in httpClientCookies() -> http::client::Cookies;
        methods()->httpClientCookies() += serviceSol() * []()
        {
            return cmt::readyFuture(createImpl<http::client::Cookies>());
        };

        // in stream2HttpServer(stream::Channel) -> http::server::Channel;
        methods()->stream2HttpServer() += serviceSol() * [](api::stream::Channel<>&& streamChannel)
        {
            return cmt::readyFuture(createImpl<http::server::Channel>(std::move(streamChannel)));
        };

        // in stream2Http2Client(stream::Channel) -> http2::client::Channel;
        methods()->stream2Http2Client() += serviceSol() * [](api::stream::Channel<>&& streamChannel)
        {
            return cmt::readyFuture(createImpl<http2::client::Channel>(std::move(streamChannel)));
        };

        // in stream2Http2Server(stream::Channel) -> http2::server::Channel;
        methods()->stream2Http2Server() += serviceSol() * [](api::stream::Channel<>&& streamChannel)
        {
            return cmt::readyFuture(createImpl<http2::server::Channel>(std::move(streamChannel)));
        };

        // in stream2Ws(stream::Channel) -> ws::Channel;
        methods()->stream2Ws() += serviceSol() * [](api::stream::Channel<>&& streamChannel)
        {
            return cmt::readyFuture(createImpl<ws::Channel>(std::move(streamChannel)));
        };

        // in agent() -> Agent;
        methods()->agent() += serviceSol() * [hostManager=_hostManager]()
        {
            return cmt::readyFuture(createImpl<Agent>(hostManager));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Factory::~Factory()
    {
        serviceSol().flush();
    }
}
