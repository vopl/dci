// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channel.hpp"

namespace dci::module::www::http::client
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(api::stream::Channel<>&& streamChannel)
        : api::http::client::Channel<>::Opposite{idl::interface::Initializer{}}
        , io::Plexus<Response, Request, false>{std::move(streamChannel), *this}
    {
        // in upgradeHttp2(www::Channel::Opposite http2ClientChannel) -> bool;
        methods()->upgradeHttp2() += _sol * [](api::Channel<>::Opposite&& /*http2ClientChannel*/)
        {
            dbgFatal("not impl");
            return cmt::readyFuture<bool>(exception::buildInstance<idl::interface::exception::MethodNotImplemented>());
        };

        // in upgradeWs(www::Channel::Opposite wsChannel) -> bool;
        methods()->upgradeWs() += _sol * [](api::Channel<>::Opposite&& /*wsChannel*/)
        {
            dbgFatal("not impl");
            return cmt::readyFuture<bool>(exception::buildInstance<idl::interface::exception::MethodNotImplemented>());
        };

        // in io(Request::Opposite, Response::Opposite);
        methods()->io() += _sol * [&](api::http::client::Request<>::Opposite&& request, api::http::client::Response<>::Opposite&& response)
        {
            io::Plexus<Response, Request, false>::emplace(std::tuple{std::move(response)}, std::tuple{std::move(request)});
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        _sol.flush();
    }
}
