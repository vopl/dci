// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::node::link::remote
{
    struct State
    {
        apit::Channel<>                     _channel;
        bool                                _byConnect {};
        idl::gen::stiac::Protocol<>         _stiacProto;
        idl::gen::stiac::LocalEdge<>        _stiacLocalEdge;
        api::remote::Payload<>::Opposite    _payloadOut;

        api::Id                     _id {};
        api::remote::Payload<>      _payload;


        State(apit::Channel<>&& channel, bool byConnect);
        void reset();
   };
}
