// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../performer.hpp"

namespace dci::module::ppn::transport::natt::mapper::awsEc2
{
    class Lookout;
    class Performer
        : public mapper::Performer
    {
    public:
        Performer(Lookout* l, const net::Ip4Endpoint& internal);
        ~Performer() override;

        bool start() override;
        bool keepalive() override;
        void stop() override;

    private:
        bool fetch();

    private:
        Lookout *           _l {};
        std::size_t         _revision {};
        net::Ip4Endpoint    _internal {};
    };
}
