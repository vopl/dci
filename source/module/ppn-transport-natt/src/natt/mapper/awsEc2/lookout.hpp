// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../lookout.hpp"
#include "../service.hpp"
#include "../performer.hpp"

namespace dci::module::ppn::transport::natt::mapper::awsEc2
{
    class Lookout
        : public mapper::Lookout
        , private mapper::Service
    {
    public:
        Lookout(Mapper* mapper);
        ~Lookout() override;

        PerformerBlank candidate(const apit::Address& internal) override;

        const net::Ip4Address& internal() const;
        const net::Ip4Address& external() const;
        std::size_t revision() const;
        cmt::Waitable& changed();

    private:
        void reveal();
        net::Ip4Address requestIp(net::stream::Client<> client, const std::string& path);

    private:
        net::Ip4Address _internal;
        net::Ip4Address _external;
        std::size_t     _revision{};
        cmt::Pulser     _changed;

    private:
        cmt::task::Owner    _tol;
        poll::Timer         _revealTicker{std::chrono::minutes{10}, true, [this]{reveal();}, &_tol};
        std::size_t         _revealDepth {};

    };
}
