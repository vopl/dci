// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::transport::natt::mapper
{
    class Performer
    {
    public:
        Performer();
        virtual ~Performer();

        virtual bool start() = 0;
        virtual bool keepalive() = 0;
        virtual void stop() = 0;

        const apit::Address& external() const;

    protected:
        apit::Address _external;
    };

    using PerformerPtr = std::unique_ptr<Performer, void(*)(Performer*)>;
}
