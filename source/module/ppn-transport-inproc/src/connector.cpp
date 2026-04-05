/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "connector.hpp"
#include "acceptor.hpp"
#include "channelBridge.hpp"

namespace dci::module::ppn::transport::inproc
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::Connector()
        : apit::inproc::Connector<>::Opposite(idl::interface::Initializer())
    {
        //in address() -> transport::Address;
        methods()->address() += serviceSol() * []
        {
            return cmt::readyFuture(apit::Address{String{"inproc://"}});
        };

        //in cost() -> real64;
        methods()->cost() += serviceSol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in rtt() -> real64;
        methods()->rtt() += serviceSol() * []
        {
            return cmt::readyFuture(real64{0});
        };

        //in bandwidth() -> real64;
        methods()->bandwidth() += serviceSol() * []
        {
            return cmt::readyFuture(std::numeric_limits<real64>::max());
        };

        //in connect(Address) -> Channel;
        methods()->connect() += serviceSol() * [](const apit::Address& address) -> cmt::Future<apit::Channel<>>
        {
            if(!utils::uri::valid<utils::uri::Inproc<>>(address.value))
            {
                return cmt::readyFuture<apit::Channel<>>(exception::buildInstance<api::BadAddress>());
            }

            Acceptor* acceptor = Acceptor::findAcceptor(address.value);
            if(!acceptor)
            {
                return cmt::readyFuture<apit::Channel<>>(exception::buildInstance<api::ConnectionRefused>());
            }

            auto pair = ChannelBridge::allocate(address);

            (*acceptor)->accepted(pair.first);
            return cmt::readyFuture(pair.second);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Connector::~Connector()
    {
        serviceSol().flush();
    }
}
