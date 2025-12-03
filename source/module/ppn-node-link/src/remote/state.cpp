// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "state.hpp"

namespace dci::module::ppn::node::link::remote
{
    State::State(apit::Channel<>&& channel, bool byConnect)
        : _channel(std::move(channel))
        , _byConnect(byConnect)
    {
    }

    void State::reset()
    {
        _channel.reset();
        _stiacProto.reset();
        _stiacLocalEdge.reset();
        _payloadOut.reset();

        //_id = api::Id{};
        _payload.reset();
    }
}
