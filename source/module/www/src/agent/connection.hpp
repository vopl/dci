// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "owningDList.hpp"

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
        : public utils::IntrusiveDlistElement<Connection>
        , public mm::heap::Allocable<Connection>
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

        uint32 id() const;
        connection::State state() const;

        std::size_t iosPerformingCount() const;
        void perform(Io* io);
        void fail(const ExceptionPtr& fail);
        const ExceptionPtr& fail() const;

        void ioCancelled(Io* io);
        void ioFailed(Io* io);
        void ioDone(Io* io);
        void ioWantClose(Io* io);

        const api::agent::log::Stream<>::Opposite& logStream();

    private:
        void idleLogic(bool forceChangedNotification4Site);
        void close();

    private:
        sbs::Owner                                  _sol;
        cmt::task::Owner                            _tol;
        Agent*                                      _agent{};
        Site*                                       _site{};
        const uint32                                _id;
        connection::State                           _state{};

        api::agent::log::Stream<>::Opposite         _logStream;

        OwningDList<Io>                             _iosPerforming;

        net::stream::Channel<>                      _netChannel;
        api::tls::client::Channel<>                 _tlsChannel;
        api::http::client::Channel<>                _httpChannel;

        ExceptionPtr                                _fail;

        std::chrono::steady_clock::time_point   _idleBound{};
        poll::Timer                             _idleTicker{std::chrono::seconds{1}, [this]{ idleLogic(false); }};
    };
}
