// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../netBased/lookout.hpp"
#include "service.hpp"

namespace dci::module::ppn::transport::natt::mapper::igdp
{
    class Lookout
        : public mapper::netBased::Lookout<Lookout, Service>
    {
    public:
        Lookout(Mapper* mapper);
        ~Lookout() override;

        cmt::Waitable* regularTicker();
        void regularTick();

    public:
        void mcastReceived(Bytes&& data, const net::Endpoint& senderEp);

    public:
        static constexpr uint16             _mcastListenPort    = 1900;
        static constexpr Array<uint8, 4>    _mcastListenAddr4   = {239,255,255,250};
        static constexpr Array<uint8, 16>   _mcastListenAddr6   = {0xff,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

        static constexpr uint16             _servicePort        = 1900;

    private:
        poll::WaitableTimer<> _regularTicker {std::chrono::milliseconds{1000*60}, true};
    };
}
