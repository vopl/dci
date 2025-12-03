// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../base.hpp"

namespace dci::module::stiac::stages::out
{
    class Compression
        : public Base
    {
        Compression(const Compression&) = delete;
        void operator=(const Compression&) = delete;

    public:
        Compression(Protocol* protocol);
        ~Compression() override;

    private:
        uint16 getWantedEmptyPrefix() const override;
        bool initialize() override;
        Bytes flushOutput() override;

    private:
        ZSTD_CStream* _zcs;
    };

    using CompressionPtr = std::unique_ptr<Compression>;
}
