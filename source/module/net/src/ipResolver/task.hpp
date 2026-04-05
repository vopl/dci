/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once
#include "pch.hpp"
#include "../utils/sockaddr.hpp"
#include <regex>

namespace dci::module::net::ipResolver
{
    class Task
    {
    public:
        ~Task();

        template <class Value>
        cmt::Future<Value> init(auto&& src);

        void doWorkInThread();
        void resolve();

    private:
        template <class Dst>
        bool fetchEndpoint(Dst& dst, addrinfo* src);

    public:
        Task*       _next4Worker {};

    private:
        String      _host;
        String      _service;

        addrinfo    _hint {};
        addrinfo *  _res {};
        int         _resCode {};

        using PromiseVar = std::variant
        <
            char,
            cmt::Promise<api::IpEndpoint>,
            cmt::Promise<api::Ip4Endpoint>,
            cmt::Promise<api::Ip6Endpoint>,
            cmt::Promise<List<api::IpEndpoint>>,
            cmt::Promise<List<api::Ip4Endpoint>>,
            cmt::Promise<List<api::Ip6Endpoint>>
        >;
        PromiseVar          _promiseVar;
        std::atomic<bool>   _canceled = false;

        poll::Awaker        _awaker;
        sbs::Owner          _sol;

    private:
        static const std::regex _regexHostService4;
        static const std::regex _regexHostService6;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Value>
    cmt::Future<Value> Task::init(auto&& src)
    {
        /*
         * ip4
         * ip6
         * domain.name
         *
         * ip4:service
         * [ip6]:service
         * domain.name:service
         *
         */

        {
            std::smatch match;
            if(std::regex_match(src, match, _regexHostService6))
            {
                _host = match[1];
                _service = match[2];
            }
            else if(std::regex_match(src, match, _regexHostService4))
            {
                _host = match[1];
                _service = match[2];
            }
            else
            {
                _host = std::forward<decltype(src)>(src);
            }
        }


        memset(&_hint, 0, sizeof(_hint));
        _hint.ai_flags = AI_ALL | AI_ADDRCONFIG;

        if constexpr(std::is_same_v<Value, api::Ip4Endpoint> || std::is_same_v<Value, List<api::Ip4Endpoint>>)
        {
            _hint.ai_family = AF_INET;
        }
        else if constexpr(std::is_same_v<Value, api::Ip6Endpoint> || std::is_same_v<Value, List<api::Ip6Endpoint>>)
        {
            _hint.ai_family = AF_INET6;
        }
        else
        {
            _hint.ai_family = AF_UNSPEC;
        }

        using Promise = cmt::Promise<Value>;
        Promise p;
        p.canceled() += _sol * [this]
        {
            _canceled.store(true, std::memory_order_release);
        };

        cmt::Future<Value> f = p.future();
        _promiseVar = std::move(p);

        _awaker.woken() += _sol * [this]
        {
            resolve();
        };

        return f;
    }
}
