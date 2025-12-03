// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channel.hpp"

namespace dci::module::www::http::server
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(api::stream::Channel<>&& streamChannel)
        : api::http::server::Channel<>::Opposite{idl::interface::Initializer{}}
        , io::Plexus<Request, Response, true>{std::move(streamChannel), *this}
    {
        // out upgradeHttp2(www::Channel::Opposite http2ServerChannel) -> bool;
        // out upgradeWs(www::Channel::Opposite wsChannel) -> bool;
        // out io(Request::Opposite, Response::Opposite);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        _sol.flush();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Channel::emitIo(api::http::server::Request<>&& request)
    {
        api::http::server::Response<> response;
        io::Plexus<Request, Response, true>::emplace(response.init2());
        methods()->io(std::move(request), std::move(response));
    }
}
