// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../performer.hpp"

namespace dci::module::ppn::transport::natt::mapper::igdp
{
    class Service;
    class Performer
        : public mapper::Performer
    {
    public:
        Performer(Service* srv, api::Protocol protocol, uint16 internalPort, const net::IpEndpoint& externalEp);
        ~Performer() override;

        bool start() override;
        bool keepalive() override;
        void stop() override;

    private:
        Service *       _srv;
        api::Protocol   _protocol;
        uint16          _internalPort{};
        net::IpEndpoint _externalEp {};
    };
}
