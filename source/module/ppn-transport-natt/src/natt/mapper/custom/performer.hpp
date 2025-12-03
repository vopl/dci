// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../performer.hpp"

namespace dci::module::ppn::transport::natt::mapper::custom
{
    class Lookout;
    class Performer
        : public mapper::Performer
    {
    public:
        Performer(Lookout* l, const apit::Address& internal);
        ~Performer() override;

        bool start() override;
        bool keepalive() override;
        void stop() override;

    private:
        bool fetch();

    private:
        Lookout *       _l;
        std::size_t     _revision{};
        apit::Address   _internal;
    };
}
