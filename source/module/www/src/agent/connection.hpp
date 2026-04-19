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
#include "rcptr.hpp"
#include "refCounted.hpp"

namespace dci::module::www
{
    class Agent;
}

namespace dci::module::www::agent::connection
{
    enum class State
    {
        pending,
        working,
        full,
        shutdown,
        fail,
        done,
    };
}

namespace dci::module::www::agent
{
    class Site;
    class Io;

    class Connection
        : public mm::heap::Allocable<Connection>
        , public RefCounted<Connection>
    {
    public:
        struct SiteData
        {
            std::size_t         _iosPerformingCount{};
            connection::State   _state{};
        } _siteData{};

    public:
        Connection(Agent* agent, Site* site, uint32 id);
        ~Connection();

        void setAgent(Agent* agent);
        void setSite(Site* site);

        uint32 id() const;
        connection::State state() const;

        std::size_t iosPerformingCount() const;
        void perform(RCPtr<Io>&& io);
        void fail(const ExceptionPtr& fail);
        const ExceptionPtr& fail() const;

        void ioCancelled(const RCPtr<Io>& io);
        void ioFailed(const RCPtr<Io>& io);
        void ioDone(const RCPtr<Io>& io);
        void ioWantClose(const RCPtr<Io>& io);

        const api::agent::log::Stream<>::Opposite& logStream();

    private:
        void idleLogic(bool notifySite);
        void close(bool notifySite = true);

        bool isWorkable() const;

    private:
        sbs::Owner                                  _sol;
        cmt::task::Owner                            _tol;
        Agent*                                      _agent{};
        Site*                                       _site{};
        const uint32                                _id;
        connection::State                           _state{};

        api::agent::log::Stream<>::Opposite         _logStream;

        std::flat_set<RCPtr<Io>>                    _iosPerforming;

        net::stream::Channel<>                      _netChannel;
        api::tls::client::Channel<>                 _tlsChannel;
        api::http::client::Channel<>                _httpChannel;

        ExceptionPtr                                _fail;

        std::chrono::steady_clock::time_point   _idleBound{};
        poll::Timer                             _idleTicker{std::chrono::seconds{1}, [this]{ idleLogic(false); }};
    };
}
