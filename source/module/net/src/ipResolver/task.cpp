// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
