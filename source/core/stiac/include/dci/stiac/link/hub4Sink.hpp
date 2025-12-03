// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include <dci/bytes.hpp>
#include "id.hpp"
#include "base.hpp"

namespace dci::stiac::link
{
    class Sink;

    class Hub4Sink
    {
    public:
        virtual ~Hub4Sink() = default;

        virtual LocalId emplaceLink(BasePtr&& link) = 0;
        virtual void finalize(Sink& sink, bytes::Alter&& buffer) = 0;
        virtual std::pair<uint32, bool> mapTuid(const std::array<uint8, 16>& tuid) = 0;
    };

}
