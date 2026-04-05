/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "host.hpp"
#include "stream/server.hpp"
#include "stream/client.hpp"
#include "stream/channel.hpp"
#include "datagram/channel.hpp"

namespace dci::module::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Host::Host()
        : api::Host<>::Opposite(idl::interface::Initializer())
        , _links(this)
        , _routes(this)
        , _ipResolver(this)
    {
        methods()->streamServer() += this * [this]()
        {
            stream::Server* s = new stream::Server{this};
            s->involvedChanged() += s * [s](bool v)
            {
                if(!v)
                {
                    delete s;
                }
            };
            return cmt::readyFuture(api::stream::Server<>(*s));
        };

        methods()->streamClient() += this * [this]()
        {
            stream::Client* c = new stream::Client{this};
            c->involvedChanged() += c * [c](bool v)
            {
                if(!v)
                {
                    delete c;
                }
            };
            return cmt::readyFuture(api::stream::Client<>(*c));
        };

        methods()->datagramChannel() += this * [this]()
        {
            datagram::Channel* c = new datagram::Channel{this};
            c->involvedChanged() += c * [c](bool v)
            {
                if(!v)
                {
                    delete c;
                }
            };
            return cmt::readyFuture(api::datagram::Channel<>(*c));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Host::~Host()
    {
        flush();

        while(!_streamServers.empty())
        {
            delete* _streamServers.begin();
        }

        while(!_streamClients.empty())
        {
            delete* _streamClients.begin();
        }

        while(!_streamChannels.empty())
        {
            delete* _streamChannels.begin();
        }

        while(!_datagramChannels.empty())
        {
            delete* _datagramChannels.begin();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::track(stream::Server* v)
    {
        dbgAssert(_streamServers.end() == _streamServers.find(v));
        _streamServers.insert(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::untrack(stream::Server* v)
    {
        dbgAssert(_streamServers.end() != _streamServers.find(v));
        _streamServers.erase(_streamServers.find(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::track(stream::Client* v)
    {
        dbgAssert(_streamClients.end() == _streamClients.find(v));
        _streamClients.insert(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::untrack(stream::Client* v)
    {
        dbgAssert(_streamClients.end() != _streamClients.find(v));
        _streamClients.erase(_streamClients.find(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::track(stream::Channel* v)
    {
        dbgAssert(_streamChannels.end() == _streamChannels.find(v));
        _streamChannels.insert(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::untrack(stream::Channel* v)
    {
        dbgAssert(_streamChannels.end() != _streamChannels.find(v));
        _streamChannels.erase(_streamChannels.find(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::track(datagram::Channel* v)
    {
        dbgAssert(_datagramChannels.end() == _datagramChannels.find(v));
        _datagramChannels.insert(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Host::untrack(datagram::Channel* v)
    {
        dbgAssert(_datagramChannels.end() != _datagramChannels.find(v));
        _datagramChannels.erase(_datagramChannels.find(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    utils::RecvBuffer* Host::getRecvBuffer()
    {
        return &_recvBuffer;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    datagram::SendBuffer* Host::getDatagramSendBuffer()
    {
        return &_datagramSendBuffer;
    }
}
