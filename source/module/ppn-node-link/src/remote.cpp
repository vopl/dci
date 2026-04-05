/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "remote.hpp"
#include "local.hpp"

namespace dci::module::ppn::node::link
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Remote::Remote(Local* local, remote::State&& state)
        : api::Remote<>::Opposite(idl::interface::Initializer{})
        , remote::State(std::move(state))
        , _local(local)
    {
        methods()->id() += this * [this]
        {
            return cmt::readyFuture(_id);
        };

        methods()->ids() += _sbsOwner4Payload * [this]
        {
            if(!_payload)
            {
                return cmt::readyFuture<Set<idl::ILid>>(exception::buildInstance<api::RemoteFailure>("no payload provided"));
            }

            return _payload->ids();
        };

        methods()->getInstance() += _sbsOwner4Payload * [this](idl::ILid ilid)
        {
            if(!_payload)
            {
                return cmt::readyFuture<idl::Interface>(exception::buildInstance<api::RemoteFailure>("no payload provided"));
            }

            return _payload->getInstance(ilid);
        };

        methods()->localAddress() += this * [this]
        {
            if(!_channel)
            {
                return cmt::readyFuture<apit::Address>(exception::buildInstance<api::ChannelFailure>("no local address available"));
            }

            return _channel->localAddress();
        };

        methods()->remoteAddress() += this * [this]
        {
            if(!_channel)
            {
                return cmt::readyFuture<apit::Address>(exception::buildInstance<api::ChannelFailure>("no remote address available"));
            }

            return _channel->remoteAddress();
        };

        methods()->originalRemoteAddress() += this * [this]()
        {
            if(!_channel)
            {
                return cmt::readyFuture<apit::Address>(exception::buildInstance<api::ChannelFailure>("no original remote address available"));
            }

            return _channel->originalRemoteAddress();
        };

        methods()->close() += this * [this]
        {
            doClose();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    RemotePtr Remote::create(Local* local, remote::State&& state)
    {
        return RemotePtr{new Remote{local, std::move(state)}};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Remote::~Remote()
    {
        _local = nullptr;
        doClose();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::start()
    {
        _channel.involvedChanged() += this * [this](bool v)
        {
            if(!v)
            {
                methods()->failed(exception::buildInstance<api::ChannelFailure>());
                doClose();
            }
        };

        _channel->failed() += this * [this](ExceptionPtr&& e)
        {
            methods()->failed(exception::buildInstance<api::ChannelFailure>(e));
            doClose();
        };

        _channel->closed() += this * [this]()
        {
            doClose();
        };

        _stiacProto.involvedChanged() += this * [this](bool v)
        {
            if(!v)
            {
                methods()->failed(exception::buildInstance<api::ProtocolFailure>());
                doClose();
            }
        };

        _stiacProto->failed() += this * [this](ExceptionPtr&& e)
        {
            methods()->failed(exception::buildInstance<api::ProtocolFailure>(e));
            doClose();
        };


        _stiacLocalEdge.involvedChanged() += this * [this](bool v)
        {
            if(!v)
            {
                methods()->failed(exception::buildInstance<api::ProtocolFailure>());
                doClose();
            }
        };

        _stiacLocalEdge->failed() += this * [this](ExceptionPtr&& e)
        {
            methods()->failed(exception::buildInstance<api::ProtocolFailure>(e));
            doClose();
        };

        _stiacLocalEdge->got() += this * [this](idl::Interface&& i)
        {
            if(i.mdLid() != api::remote::Payload<>::lid())
            {
                methods()->failed(exception::buildInstance<api::RemoteFailure>("unknown interface received from remote instead of Payload"));
                return cmt::readyFuture(None{});
            }

            _sbsOwner4Payload.flush();
            _payload = std::move(i);

            if(!_payload)
            {
                methods()->failed(exception::buildInstance<api::RemoteFailure>("null Payload interface received from remote"));
                return cmt::readyFuture(None{});
            }

            setupPayload();
            return cmt::readyFuture(None{});
        };

        setupPayloadOut();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const remote::State& Remote::state() const
    {
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::payloadOutIdsChanged()
    {
        dbgAssert(_payloadOut);
       _payloadOut->idsChanged();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::setupPayload()
    {
        dbgAssert(_payload);
        {
            _payload.involvedChanged() += _sbsOwner4Payload * [this](bool v)
            {
                if(!v)
                {
                    methods()->failed(exception::buildInstance<api::RemoteFailure>("payload uninvolved"));
                    doClose();
                }
            };

            _payload->idsChanged() += _sbsOwner4Payload * [this]
            {
                methods()->idsChanged();
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::setupPayloadOut()
    {
        dbgAssert(_payloadOut);
        {
            _payloadOut.involvedChanged() += this * [this](bool v)
            {
                if(!v)
                {
                    methods()->failed(exception::buildInstance<api::RemoteFailure>("payloadOut uninvolved"));
                    doClose();
                }
            };

            _payloadOut->ids() += this * [this]
            {
                return cmt::readyFuture(_local->payloadIds());
            };

            _payloadOut->getInstance() += this * [this](idl::ILid ilid)
            {
                return _local->getPayloadInstance(this, ilid);
            };
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Remote::doClose()
    {
        flush();
        _sbsOwner4Payload.flush();

        if(_channel)
        {
            _channel->close();
            methods()->closed();
        }

        remote::State::reset();

        if(_local)
        {
            std::exchange(_local, nullptr)->remoteClosed(this);
        }
    }
}
