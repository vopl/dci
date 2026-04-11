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
#include "rcptr.hpp"

namespace dci::module::www
{
    class Agent;
}

namespace dci::module::www::agent
{
    class Connection;
    class Io;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Site
    {
    public:
        Site(Agent* agent, site::Endpoint&& endpoint);
        ~Site();

        const site::Endpoint& endpoint() const;
        void perform(RCPtr<Io> io);
        void fail(const ExceptionPtr& fail);

    private:
        friend Connection;
        friend Io;

        void connectionChanged(RCPtr<Connection> connection);
        void ioCancelled(RCPtr<Io> io);
        void ioFailed(RCPtr<Io> io);
        void flowLogicStep();
        void connectLogic();

    private:
        Agent*                      _agent{};
        const site::Endpoint        _endpoint;

        uint32                      _topConnectionId{};
        std::set<uint32>            _unusedConnectionIds;

        std::chrono::steady_clock::time_point
                                    _lastConnectMoment{};
        poll::Timer                 _connectTicker{std::chrono::seconds{1}, [this]{ connectLogic(); }};

        std::flat_set<RCPtr<Connection>>    _connectionsPending;
        std::flat_set<RCPtr<Connection>>    _connectionsWorking;
        std::flat_set<RCPtr<Connection>>    _connectionsFull;
        std::flat_set<RCPtr<Connection>>    _connectionsShutdown;

        std::size_t                 _connectionsWorkingRoundRobin{};

        std::flat_set<RCPtr<Io>>    _iosPending;
        std::size_t                 _iosPerformingCount{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool operator<(const agent::Site& a,             const agent::Site& b);
    bool operator<(const agent::Site& a,             const agent::site::Endpoint& b);
    bool operator<(const agent::site::Endpoint& a,   const agent::Site& b);
}
