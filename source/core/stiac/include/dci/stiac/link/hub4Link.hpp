// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "id.hpp"

namespace dci::stiac::link
{
    class Sink;

    class Hub4Link
    {
    public:
        virtual ~Hub4Link() = default;
        virtual Sink makeSink(Id id) = 0;

        enum UninvolvingFlags
        {
            uf_beginRemove  = 0x001,
            uf_endRemove    = 0x002,
            uf_remove       = 0x004,

            uf_sendBegin    = 0x010,
            uf_sendEnd      = 0x020,
        };

        virtual void linkUninvolved(Id id, int uf/* = uf_beginRemove | uf_sendBegin*/) = 0;
    };
}
