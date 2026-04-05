/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "channel.hpp"
#include "endpoint2Address.hpp"

namespace dci::module::ppn::transport::net
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(apit::Address&& originalRemoteAddress, idl::gen::net::stream::Channel<>&& netStreamChannel)
        : apit::Channel<>::Opposite(idl::interface::Initializer{})
        , _originalRemoteAddress(std::move(originalRemoteAddress))
        , _netStreamChannel(std::move(netStreamChannel))
    {
        methods()->localAddress() += this * [this]()
        {
            return _netStreamChannel->localEndpoint().chain<apit::Address>(*this, [](auto in, auto& out)
            {
                if(in.resolvedValue())
                {
                    out.resolveValue(endpoint2Address(in.detachValue()));
                }
                else if(in.resolvedException())
                {
                    out.resolveException(in.detachException());
                }
                else
                {
                    out.resolveCancel();
                }
            });
        };

        methods()->remoteAddress() += this * [this]()
        {
            return _netStreamChannel->remoteEndpoint().chain<apit::Address>(*this, [](auto in, auto& out)
            {
                if(in.resolvedValue())
                {
                    out.resolveValue(endpoint2Address(in.detachValue()));
                }
                else if(in.resolvedException())
                {
                    out.resolveException(in.detachException());
                }
                else
                {
                    out.resolveCancel();
                }
            });
        };

        methods()->originalRemoteAddress() += this * [this]()
        {
            return cmt::readyFuture(_originalRemoteAddress);
        };

        methods()->unlockInput() += this * [this]() -> void
        {
            return _netStreamChannel->startReceive();
        };

        methods()->lockInput() += this * [this]() -> void
        {
             return _netStreamChannel->stopReceive();
        };

        _netStreamChannel->failed() += this * [this](auto&& e)
        {
            methods()->failed(std::forward<decltype(e)>(e));
        };

        methods()->close() += this * [this]() -> void
        {
            _netStreamChannel->close();
        };

        _netStreamChannel->closed() += this * [this]()
        {
            methods()->closed();
        };

        _netStreamChannel->received() += this * [this](auto&& data)
        {
            methods()->input(std::forward<decltype(data)>(data));
        };

        methods()->output() += this * [this](auto&& data)
        {
            _netStreamChannel->send(std::forward<decltype(data)>(data));
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        flush();
    }
}
