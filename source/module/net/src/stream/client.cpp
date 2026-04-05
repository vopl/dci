/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "client.hpp"
#include "../host.hpp"

namespace dci::module::net::stream
{
    Client::Client(Host* host)
        : api::stream::Client<>::Opposite{idl::interface::Initializer{}}
        , _host(host)
        , _binded(false)
    {
        _host->track(this);

        methods()->setOption() += this * [&](const api::Option& op)
        {
            pushOption(op);
            return cmt::readyFuture(None{});
        };

        methods()->bind() += this * [this](auto&& endpoint)
        {
            _bindEndpoint = std::forward<decltype(endpoint)>(endpoint);
            _binded = true;
            return cmt::readyFuture(None{});
        };

        methods()->connect() += this * [this](auto&& endpoint)
        {
            stream::Channel* c = new stream::Channel{_host, {}, _bindEndpoint, api::Endpoint(std::forward<decltype(endpoint)>(endpoint))};
            c->involvedChanged() += c * [c](bool v)
            {
                if(!v)
                {
                    delete c;
                }
            };

            c->pushOptions(options());
            cmt::Future<api::stream::Channel<>> res = c->connect(_binded);
            res.then() += c * [c](cmt::Future<api::stream::Channel<>> in)
            {
                if(!in.resolvedValue())
                {
                    delete c;
                }
            };

            return res;
        };
    }

    Client::~Client()
    {
        sbs::Owner::flush();
        _host->untrack(this);
    }
}
