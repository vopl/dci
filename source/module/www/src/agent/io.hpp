/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "pch.hpp"
#include "site/endpoint.hpp"
#include "refCounted.hpp"
#include "rcptr.hpp"

namespace dci::module::www
{
    class Agent;
}

namespace dci::module::www::agent
{
    class Site;
    class Connection;

    class Io
        : public mm::heap::Allocable<Io>
        , public RefCounted<Io>
    {
    public:
        Io(Agent* agent, const api::http::client::Cookies<>& cookies, api::agent::io::Request&& request);
        ~Io();

        cmt::Future<api::agent::io::Response> future();
        bool done();
        void fail(const ExceptionPtr& fail);
        std::expected<site::Endpoint, ExceptionPtr> calculateSiteEndpoint();

        void setAgent(Agent* agent);
        void setSite(Site* site);
        void setConnection(Connection* connection);
        void perform(const api::http::client::Channel<>& httpChannel);

        bool started();

    private:
        void log(auto&& f);
        void logHeaders(const primitives::List<api::http::Header>& headers, bool done, std::string prefix);
        void logData(const Bytes& data, bool done, std::string prefix);

        void onResponseDone();

        bool isOrphan() const;


    private:
        sbs::Owner                              _sol;
        cmt::task::Owner                        _tol;
        Agent*                                  _agent{};
        Site*                                   _site{};
        Connection*                             _connection{};

        api::http::client::Cookies<>            _cookies;
        api::agent::io::Request                 _request;
        dci::utils::uri::WWW<std::string_view>  _uriParsed;

        cmt::Promise<api::agent::io::Response>  _responsePromise;
        api::agent::io::Response                _responseAccumuler;

    private:
        bool                                    _started{};
        api::http::client::Response<>           _httpResponse;
    };
}
