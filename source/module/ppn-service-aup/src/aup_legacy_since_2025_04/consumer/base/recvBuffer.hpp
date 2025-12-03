// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::ppn::service::aup_legacy_since_2025_04::consumer::base
{
    class RecvBuffer
    {
        RecvBuffer(const RecvBuffer&) = delete;
        RecvBuffer(RecvBuffer&&) = delete;

        void operator=(const RecvBuffer&) = delete;
        void operator=(RecvBuffer&&) = delete;

    public:
        RecvBuffer(uint32 ramBound);
        ~RecvBuffer();

        bool                        push(Bytes&& payload);
        uint32                      payloadSize();
        bool                        hasFile();
        Bytes                       detachBytes();
        std::FILE*                  getFile();
        void                        reset();

    private:
        bool pushFs(Bytes&& payload);

    private:
        uint32      _ramBound{};
        uint32      _payloadSize{};
        Bytes       _ramPayload;
        std::string _fsPayloadPath;
        std::FILE*  _fsPayload{};
    };
}
