// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
