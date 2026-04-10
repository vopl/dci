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

        Tuple<api::stream::Channel<> /*local*/, api::stream::Channel<>::Opposite /*remote*/> makeHookChannelsNet(sbs::Owner& sol);
        Tuple<api::stream::Channel<> /*local*/, api::stream::Channel<>::Opposite /*remote*/> makeHookChannelsHttp(sbs::Owner& sol);
        static Tuple<api::stream::Channel<> /*local*/, api::stream::Channel<>::Opposite /*remote*/> makeHookChannels(sbs::Owner& sol, const List<api::agent::Hook<>::Opposite>& hooks);
        static void interconnect(const api::stream::Channel<>::Opposite& remote, sbs::Owner& sol, const api::stream::Channel<>& local);

        template <class I>
        cmt::Future<I> getDependency();

    private:
        cmt::task::Owner                        _tol;
        host::Manager*                          _hostManager{};

        Set<api::agent::DependenciesFactory<>::Opposite>
                                                _dependenciesFactories;

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

        List<api::agent::Hook<>::Opposite> _hooksNet;
        List<api::agent::Hook<>::Opposite> _hooksHttp;

        using Ios = agent::OwningDList<agent::Io>;

        using Sites = bmi::multi_index_container<
            agent::Site,
            bmi::indexed_by<bmi::ordered_unique<bmi::identity<agent::Site>, std::less<void>>>
        >;

        Variant<Ios, Sites> _staff;
    };
}
