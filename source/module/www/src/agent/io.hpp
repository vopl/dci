// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "site/endpoint.hpp"

namespace dci::module::www
{
    class Agent;
}

namespace dci::module::www::agent
{
    class Site;
    class Connection;

    class Io
        : public dci::utils::IntrusiveDlistElement<Io>
        , public mm::heap::Allocable<Io>
    {
    public:
        Io(const api::http::client::Cookies<>& cookies, api::agent::io::Request&& request);
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

    private:
        sbs::Owner                              _sol;
        cmt::task::Owner                        _tol;
        Agent*                                  _agent{};
        Site*                                   _site{};
        Connection*                             _connection{};

        api::http::client::Cookies<>            _cookies;
        api::agent::io::Request                 _request;
        dci::utils::uri::WWW<std::string_view>  _uriParsed;

        cmt::Promise<api::agent::io::Response> _responsePromise;
        api::agent::io::Response                _responseAccumuler;

    private:
        bool                                    _started{};
        api::http::client::Response<>           _httpResponse;

    private:
        using AliveMarker = std::shared_ptr<bool>;
        AliveMarker _aliveMarker{std::make_shared<bool>(true)};
    };
}
