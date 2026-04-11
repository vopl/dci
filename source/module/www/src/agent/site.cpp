/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "site.hpp"
#include "../agent.hpp"
#include "connection.hpp"
#include "io.hpp"

namespace dci::module::www::agent
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Site::Site(Agent* agent, site::Endpoint&& endpoint)
        : _agent{agent}
        , _endpoint{std::move(endpoint)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Site::~Site()
    {
        ExceptionPtr stopFail = exception::buildInstance<api::agent::Stopped>();
        auto clear = [&](auto& container)
        {
            auto copy{std::move(container)};
            for(const auto& e : copy)
            {
                e->setAgent({});
                e->setSite({});
                e->fail(stopFail);
            }
        };

        clear(_iosPending);

        clear(_connectionsPending);
        clear(_connectionsWorking);
        clear(_connectionsFull);
        clear(_connectionsShutdown);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const site::Endpoint& Site::endpoint() const
    {
        return _endpoint;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::perform(RCPtr<Io> io)
    {
        io->setSite(this);
        _iosPending.insert(std::move(io));
        flowLogicStep();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::fail(const ExceptionPtr& fail)
    {
        auto callFail = [&](const auto& container)
        {
            auto copy{container};
            for(const auto& e : copy)
                e->fail(fail);
        };

        callFail(_connectionsPending);
        callFail(_connectionsWorking);
        callFail(_connectionsFull);
        callFail(_connectionsShutdown);

        callFail(_iosPending);

        _agent->siteDone(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::connectionChanged(RCPtr<Connection> connection)
    {
        Connection::SiteData& siteData = connection->_siteData;

        std::size_t newIosPerformingCount = connection->iosPerformingCount();
        if(newIosPerformingCount != siteData._iosPerformingCount)
        {
            _iosPerformingCount -= siteData._iosPerformingCount;
            siteData._iosPerformingCount = newIosPerformingCount;
            _iosPerformingCount += siteData._iosPerformingCount;
        }

        connection::State nextState = connection->state();

        if(nextState == siteData._state)
            return;

        switch(siteData._state)
        {
        case connection::State::pending:
            _connectionsPending.erase(connection);
            break;
        case connection::State::working:
            _connectionsWorking.erase(connection);
            break;
        case connection::State::full:
            _connectionsFull.erase(connection);
            break;
        case connection::State::shutdown:
            _connectionsShutdown.erase(connection);
            break;
        default:
            std::unreachable();
        }

        siteData._state = nextState;
        switch(siteData._state)
        {
        case connection::State::pending:
            _connectionsPending.emplace(std::move(connection));
            break;
        case connection::State::working:
            _connectionsWorking.emplace(std::move(connection));
            flowLogicStep();
            break;
        case connection::State::full:
            _connectionsFull.emplace(std::move(connection));
            break;
        case connection::State::shutdown:
            _connectionsShutdown.emplace(std::move(connection));
            break;
        case connection::State::done:
        case connection::State::fail:

            {
                if(connection->id() == _topConnectionId)
                {
                    --_topConnectionId;
                    if(!_unusedConnectionIds.empty())
                    {
                        while(!_unusedConnectionIds.empty() && *--_unusedConnectionIds.end() == _topConnectionId)
                        {
                            --_topConnectionId;
                            _unusedConnectionIds.erase(--_unusedConnectionIds.end());
                        }
                    }
                }
                else
                    _unusedConnectionIds.insert(connection->id());

                ExceptionPtr fail;
                if(connection::State::fail == siteData._state)
                    fail = connection->fail();

                connection.reset();
                flowLogicStep();

                break;
            }

        default:
            std::unreachable();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::ioCancelled(RCPtr<Io> io)
    {
        _iosPending.erase(io);
        flowLogicStep();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::ioFailed(RCPtr<Io> io)
    {
        _iosPending.erase(io);
        flowLogicStep();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::flowLogicStep()
    {
        if(_connectionsPending.empty() && _connectionsWorking.empty() && _connectionsFull.empty() && _connectionsShutdown.empty() && _iosPending.empty())
        {
            dbgAssert(!_iosPerformingCount);
            _agent->siteDone(this);
            return;
        }

        if(!_iosPending.empty() &&
           _agent->_maxIoPerformingPerSite > _iosPerformingCount &&
           !_connectionsWorking.empty())
        {
            std::size_t _connectionsWorkingRoundRobin{};

            if(++_connectionsWorkingRoundRobin >= _connectionsWorking.size())
                _connectionsWorkingRoundRobin = 0;
            RCPtr<Connection> connection = *(_connectionsWorking.begin() + _connectionsWorkingRoundRobin);

            RCPtr<Io> io;
            {
                auto ioIter = _iosPending.end();
                --ioIter;
                io = *ioIter;
                _iosPending.erase(ioIter);
            }

            connection->perform(std::move(io));

            flowLogicStep();
            return;
        }

        connectLogic();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::connectLogic()
    {
        if(_iosPending.empty())
            return;

        std::size_t connectionsCount =
                _connectionsPending.size() +
                _connectionsWorking.size() +
                _connectionsFull.size();

        if(_agent->_maxConnectionsPerSite <= connectionsCount)
            return;

        std::chrono::steady_clock::time_point now{std::chrono::steady_clock::now()};
        constexpr auto interval = std::chrono::seconds{1};
        if(now < _lastConnectMoment + interval)
        {
            _connectTicker.interval(_lastConnectMoment + interval - now + std::chrono::milliseconds{1});
            _connectTicker.start();
            return;
        }

        _lastConnectMoment = now;

        uint32 id;
        {
            if(_unusedConnectionIds.empty())
            {
                ++_topConnectionId;
                id = _topConnectionId;
            }
            else
            {
                id = *_unusedConnectionIds.begin();
                _unusedConnectionIds.erase(_unusedConnectionIds.begin());
            }

        }

        _connectionsPending.emplace(RCPtr<Connection>(new Connection(_agent, this, id)));
        flowLogicStep();
        return;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool operator<(const agent::Site& a, const agent::Site& b)
    {
        return a.endpoint() < b.endpoint();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool operator<(const agent::Site& a, const agent::site::Endpoint& b)
    {
        return a.endpoint() < b;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool operator<(const agent::site::Endpoint& a, const agent::Site& b)
    {
        return a < b.endpoint();
    }
}
