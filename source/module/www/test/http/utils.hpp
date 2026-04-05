/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/test.hpp>
#include <dci/host.hpp>
#include "www.hpp"

using namespace dci;
using namespace dci::host;
using namespace dci::idl;
using namespace dci::idl::gen;

/////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
namespace testing::internal
{
    template <>
    inline void PrintTo<dci::Bytes>(const dci::Bytes& value, ::std::ostream* os)
    {
        *os << value.toString();
    }
}

namespace http::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline
    std::tuple<
        www::stream::Channel<>,
        www::stream::Channel<>
    > interconnectWwwStream()
    {

        www::stream::Channel<> serverFront, clientFront;
        www::stream::Channel<>::Opposite serverBack = serverFront.init2();
        www::stream::Channel<>::Opposite clientBack = clientFront.init2();

        struct Link
        {
            bool        _receiveStrarted{};
            dci::Bytes  _receiveData{};
            bool        _closed{};
            idl::interface::Generic<false> _weakDst;
            www::stream::Channel<>::Opposite dst()
            {
                return _weakDst;
            }
        };

        auto interconnect = [](std::shared_ptr<Link> link, www::stream::Channel<>::Opposite src, www::stream::Channel<>::Opposite dst)
        {
            link->_weakDst = dst.weak();

            // in  send                (bytes);
            src->send() += [link](dci::Bytes&& data)
            {
                if(auto dst = link->dst())
                {
                    link->_receiveData.end().write(std::move(data));
                    if(link->_receiveStrarted && !link->_receiveData.empty())
                        dst->received(std::move(link->_receiveData));
                }
            };

            // in  startReceive        ();
            src->startReceive() += [link]()
            {
                if(auto dst = link->dst())
                {
                    link->_receiveStrarted = true;
                    if(!link->_receiveData.empty())
                        dst->received(std::move(link->_receiveData));
                }
            };

            // in  stopReceive         ();
            src->stopReceive() += [link]()
            {
                if(auto dst = link->dst())
                {
                    link->_receiveStrarted = false;
                }
            };

            // out received            (bytes);
            // in  shutdown            ();
            src->shutdown() += [link]()
            {
                if(auto dst = link->dst())
                {
                    link->_receiveStrarted = false;
                    if(!link->_closed)
                    {
                        link->_closed = true;
                        dst->closed();
                    }
                }
            };

            // out failed(exception);
            // out closed();
            // in close();
            src->close() += [link]()
            {
                if(auto dst = link->dst())
                {
                    link->_receiveStrarted = false;
                    if(!link->_closed)
                    {
                        link->_closed = true;
                        dst->closed();
                    }
                }
            };
        };

        interconnect(std::make_shared<Link>(), serverBack, clientBack);
        interconnect(std::make_shared<Link>(), clientBack, serverBack);

        return {clientFront, serverFront};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline
    std::tuple<
        www::http::client::Channel<>,
        www::http::server::Channel<>
    > interconnectWwwHttp()
    {
        Manager* manager = testManager();
        www::Factory<> wwwFactory = *manager->createService<www::Factory<>>();

        auto [client, server] = interconnectWwwStream();

        return
        {
            *wwwFactory->stream2HttpClient(client),
            *wwwFactory->stream2HttpServer(server)
        };
    }
}
