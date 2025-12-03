// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../pch.hpp"

namespace dci::module::stiac::localEdge
{
    class Input
        : public dci::stiac::link::Hub4Source
    {
    public:
        Input();
        ~Input() override;

        void append(Bytes&& data);
        bool empty() const;

        link::Source makeSource();

        bool emplaceLink(link::BasePtr&& link, link::RemoteId remoteId) override = 0;
        void finalize(link::Source& source, bytes::Alter&& buffer) override;

        bool mapTuid(uint32& mapped, const std::array<uint8, 16>& tuid) override;
        bool unmapTuid(const uint32& mapped, std::array<uint8, 16>& tuid) override;

    private:
        Bytes _data;

        bool _hasActiveSource = false;

        using TuidMapFwd = std::map<std::array<uint8, 16>, uint32>;
        using TuidMapBwd = std::vector<std::array<uint8, 16>>;
        TuidMapFwd _tuidMapFwd;
        TuidMapBwd _tuidMapBwd;
    };
}
