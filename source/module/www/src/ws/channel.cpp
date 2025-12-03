// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "channel.hpp"

namespace dci::module::www::ws
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::Channel(api::stream::Channel<>&& streamChannel)
        : api::ws::Channel<>::Opposite{idl::interface::Initializer{}}
        , _streamChannel{std::move(streamChannel)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Channel::~Channel()
    {
        serviceSol().flush();
    }
}
