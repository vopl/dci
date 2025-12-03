// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/primitives.hpp>
#include <dci/bytes.hpp>
#include "id.hpp"
#include "base.hpp"

namespace dci::stiac::link
{
    class Source;

    class Hub4Source
    {
    public:
        virtual ~Hub4Source() = default;

        virtual bool emplaceLink(BasePtr&& link, RemoteId remoteId) = 0;
        virtual void finalize(Source& source, bytes::Alter&& buffer) = 0;
        virtual bool mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid) = 0;
        virtual bool unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid) = 0;
    };

}
