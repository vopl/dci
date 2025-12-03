// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../netBased/lookout.hpp"
#include "service.hpp"

namespace dci::module::ppn::transport::natt::mapper::pmpPcp
{
    class Lookout
        : public mapper::netBased::Lookout<Lookout, Service>
    {
    public:
        Lookout(Mapper* mapper, bool usePmp = true, bool usePcp = true);
        ~Lookout() override;

        bool usePmp() const;
        bool usePcp() const;

    public:
        void mcastReceived(Bytes&& data, const net::Endpoint& srvEp);

    public:
        static constexpr uint16             _mcastListenPort    = 5350;
        static constexpr Array<uint8, 4>    _mcastListenAddr4   = {224,0,0,1};
        static constexpr Array<uint8, 16>   _mcastListenAddr6   = {0xff,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

        static constexpr uint16             _servicePort        = 5351;

    private:
        bool _usePmp {};
        bool _usePcp {};
    };
}
