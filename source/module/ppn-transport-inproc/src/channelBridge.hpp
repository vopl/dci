// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "channelBridge/side.hpp"

namespace dci::module::ppn::transport::inproc
{
    class ChannelBridge
        : private mm::heap::Allocable<ChannelBridge>
    {
    public:
        static std::pair<apit::Channel<>, apit::Channel<>> allocate(const apit::Address& address);

    private:
        ChannelBridge(const apit::Address& address);
        ~ChannelBridge();

        uint32  _useCounter {2};

    private:
        channelBridge::Side _s1;
        channelBridge::Side _s2;
    };
}
