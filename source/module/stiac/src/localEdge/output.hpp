// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../pch.hpp"

namespace dci::module::stiac::localEdge
{
    class Output
        : public link::Hub4Sink
    {
    public:
        Output(Bytes& data);
        ~Output() override;

        link::Sink makeSink(uint32 reserveIfCan);

        link::LocalId emplaceLink(link::BasePtr&& link) override = 0;
        void finalize(link::Sink& sink, bytes::Alter&& buffer) override;
        std::pair<uint32, bool> mapTuid(const std::array<uint8, 16>& tuid) override;

    private:
        Bytes& _data;

        bool _hasActiveSink = false;
        uint32 _reserved = 0;

        using TuidMap = std::map<std::array<uint8, 16>, uint32>;
        TuidMap _tuidMap;
    };
}
