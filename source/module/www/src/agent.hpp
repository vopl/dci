// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "agent/site.hpp"
#include "agent/io.hpp"
#include "agent/owningDList.hpp"

namespace dci::module::www
{
    namespace bmi = boost::multi_index;

    class Agent
        : public host::module::ServiceBase<Agent>
        , public api::Agent<>::Opposite
    {
    public:
        Agent(host::Manager* hostManager);
        ~Agent();

    private:
        cmt::Future<api::agent::io::Response> io(api::agent::io::Request&& request);

    private:
        friend agent::Site;
        friend agent::Connection;
        friend agent::Io;

        const api::agent::log::Source<>::Opposite& logSink() const;

        void siteDone(agent::Site* site);
        void ioCancelled(agent::Io* io);
        void ioFailed(agent::Io* io);

    private:
        void fail(ExceptionPtr&& fail = {});
        void io2Site(agent::Io* io);

    private:
        cmt::task::Owner                        _tol;
        host::Manager*                          _hostManager{};

        net::Host<>                             _netHost;
        net::stream::Client<>                   _netStreamClient;
        api::Factory<>                          _wwwFactory;
        cmt::Promise<api::tls::setup::Trust<>>  _wwwTlsSetupTrust;
        api::Tls<>                              _wwwTls;
        api::http::client::Cookies<>            _cookies;
        api::agent::log::Source<>::Opposite     _logSink;

        bool                                    _ready{};
        ExceptionPtr                            _fail;

        uint32 _maxConnectionsPerSite       {1};
        uint32 _idleConnectionTimeoutMs     {1000*60*60*3};
        uint32 _maxIoPerformingPerConection {1};
        uint32 _maxIoPerformingPerSite      {~uint32{}};

        using Ios = agent::OwningDList<agent::Io>;

        using Sites = bmi::multi_index_container<
            agent::Site,
            bmi::indexed_by<bmi::ordered_unique<bmi::identity<agent::Site>, std::less<void>>>
        >;

        Variant<Ios, Sites> _staff;
    };
}
