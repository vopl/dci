// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "owningDList.hpp"
#include "site/endpoint.hpp"

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
        void perform(Io* io);
        void fail(const ExceptionPtr& fail);

    private:
        friend Connection;
        friend Io;

        void connectionChanged(Connection* connection);
        void ioCancelled(Io* io);
        void ioFailed(Io* io);
        void flowLogicStep();
        void connectLogic();

    private:
        Agent*                  _agent{};
        const site::Endpoint    _endpoint;

        uint32                  _topConnectionId{};
        std::set<uint32>        _unusedConnectionIds;

        std::chrono::steady_clock::time_point   _lastConnectMoment{};
        poll::Timer                             _connectTicker{std::chrono::seconds{1}, [this]{ connectLogic(); }};


        OwningDList<Connection> _connectionsPending;
        OwningDList<Connection> _connectionsWorking;
        OwningDList<Connection> _connectionsFull;
        OwningDList<Connection> _connectionsShutdown;

        OwningDList<Io>         _iosPending;
        std::size_t             _iosPerformingCount{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool operator<(const agent::Site& a,             const agent::Site& b);
    bool operator<(const agent::Site& a,             const agent::site::Endpoint& b);
    bool operator<(const agent::site::Endpoint& a,   const agent::Site& b);
}
