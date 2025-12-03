// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
        _iosPending.clear();
        _connectionsPending.clear();
        _connectionsWorking.clear();
        _connectionsFull.clear();
        _connectionsShutdown.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const site::Endpoint& Site::endpoint() const
    {
        return _endpoint;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::perform(Io* io)
    {
        io->setSite(this);
        _iosPending.push(io);
        flowLogicStep();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::fail(const ExceptionPtr& fail)
    {
        _connectionsPending .each([&](Connection* connection) { connection->fail(fail); });
        _connectionsWorking .each([&](Connection* connection) { connection->fail(fail); });
        _connectionsFull    .each([&](Connection* connection) { connection->fail(fail); });
        _connectionsShutdown.each([&](Connection* connection) { connection->fail(fail); });

        _iosPending.each([&](Io* io) { io->fail(fail); });

        _agent->siteDone(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::connectionChanged(Connection* connection)
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
            _connectionsPending.release(connection);
            break;
        case connection::State::working:
            _connectionsWorking.release(connection);
            break;
        case connection::State::full:
            _connectionsFull.release(connection);
            break;
        case connection::State::shutdown:
            _connectionsShutdown.release(connection);
            break;
        default:
            std::unreachable();
        }

        siteData._state = nextState;
        switch(siteData._state)
        {
        case connection::State::pending:
            _connectionsPending.push(connection);
            break;
        case connection::State::working:
            _connectionsWorking.push(connection);
            flowLogicStep();
            break;
        case connection::State::full:
            _connectionsFull.push(connection);
            break;
        case connection::State::shutdown:
            _connectionsShutdown.push(connection);
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
            }

            delete connection;
            flowLogicStep();
            break;
        default:
            std::unreachable();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::ioCancelled(Io* io)
    {
        _iosPending.erase(io);
        flowLogicStep();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Site::ioFailed(Io* io)
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
            Connection* connection = _connectionsWorking.first();

            {// round robin
                _connectionsWorking.release(connection);
                _connectionsWorking.push(connection);
            }

            Io* io = _iosPending.first();
            _iosPending.release(io);

            connection->perform(io);

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
                _connectionsPending.count() +
                _connectionsWorking.count() +
                _connectionsFull.count();

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

        _connectionsPending.push(new Connection(_agent, this, id));
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
