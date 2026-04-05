/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "task.hpp"
#include "../utils/makeError.hpp"

namespace dci::module::net::ipResolver
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::regex Task::_regexHostService4{"^([^\\:]+)(?:\\:(.*))?$", std::regex::optimize};

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const std::regex Task::_regexHostService6{"^\\[([^\\]]*)\\](?:\\:(.*))?$", std::regex::optimize};

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Task::~Task()
    {
        _sol.flush();
        if(_res)
        {
            freeaddrinfo(_res);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Task::doWorkInThread()
    {
        dci::utils::AtScopeExit continuation{[this]{_awaker.wakeup();}};

        if(_canceled.load(std::memory_order_consume))
        {
            return;
        }

        dbgAssert(!_res);
        _resCode = getaddrinfo(
                    _host.data(),
                    _service.data(),
                    &_hint,
                    &_res);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    bool Task::fetchEndpoint<api::Ip4Endpoint>(api::Ip4Endpoint& dst, addrinfo* src)
    {
        if(!src)
        {
            return false;
        }

        if(utils::sockaddr::convert(src->ai_addr, src->ai_addrlen, dst))
        {
            return true;
        }

        return fetchEndpoint(dst, src->ai_next);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    bool Task::fetchEndpoint<api::Ip6Endpoint>(api::Ip6Endpoint& dst, addrinfo* src)
    {
        if(!src)
        {
            return false;
        }

        if(utils::sockaddr::convert(src->ai_addr, src->ai_addrlen, dst))
        {
            return true;
        }

        return fetchEndpoint(dst, src->ai_next);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    bool Task::fetchEndpoint<api::IpEndpoint>(api::IpEndpoint& dst, addrinfo* src)
    {
        {
            api::Ip4Endpoint tmp;
            if(fetchEndpoint(tmp, src))
            {
                dst = tmp;
                return true;
            }
        }

        {
            api::Ip6Endpoint tmp;
            if(fetchEndpoint(tmp, src))
            {
                dst = tmp;
                return true;
            }
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    bool Task::fetchEndpoint<List<api::Ip4Endpoint>>(List<api::Ip4Endpoint>& dst, addrinfo* src)
    {
        if(!src)
        {
            return false;
        }

        {
            api::Ip4Endpoint tmp;
            if(utils::sockaddr::convert(src->ai_addr, src->ai_addrlen, tmp))
            {
                if(dst.end() == std::find(dst.begin(), dst.end(), tmp))
                    dst.emplace_back(tmp);
                fetchEndpoint(dst, src->ai_next);
                return true;
            }
        }

        return fetchEndpoint(dst, src->ai_next);
    }


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    bool Task::fetchEndpoint<List<api::Ip6Endpoint>>(List<api::Ip6Endpoint>& dst, addrinfo* src)
    {
        if(!src)
        {
            return false;
        }

        {
            api::Ip6Endpoint tmp;
            if(utils::sockaddr::convert(src->ai_addr, src->ai_addrlen, tmp))
            {
                if(dst.end() == std::find(dst.begin(), dst.end(), tmp))
                    dst.emplace_back(tmp);
                fetchEndpoint(dst, src->ai_next);
                return true;
            }
        }

        return fetchEndpoint(dst, src->ai_next);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    bool Task::fetchEndpoint<List<api::IpEndpoint>>(List<api::IpEndpoint>& dst, addrinfo* src)
    {
        List<api::Ip4Endpoint> dst4;
        List<api::Ip6Endpoint> dst6;

        fetchEndpoint(dst4, src);
        fetchEndpoint(dst6, src);

        for(auto& a : dst4) dst.push_back(a);
        for(auto& a : dst6) dst.push_back(a);

        return !dst.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Task::resolve()
    {
        dci::utils::AtScopeExit continuation{[this]{delete this;}};

        if(_canceled.load(std::memory_order_consume))
        {
            return;
        }

        std::visit([&](auto& p)
        {
            if constexpr(std::is_same_v<char&, decltype(p)>)
            {
                return;
            }
            else
            {
                if(p.resolved())
                {
                    return;
                }

                if(_resCode)
                {
                    p.resolveException(utils::makeError<api::ResolveError>(gai_strerror(_resCode)));
                    return;
                }

                typename std::remove_reference_t<decltype(p)>::Value v {};
                if(fetchEndpoint(v, _res))
                {
                    p.resolveValue(std::move(v));
                }
                else
                {
                    p.resolveException(utils::makeError<api::ResolveError>("unable to fetch endpoint info"));
                }
            }
        }, _promiseVar);
    }
}
