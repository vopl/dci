// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../base.hpp"

namespace dci::module::stiac::stages::out
{
    class Cutting
        : public Base
    {
        Cutting(const Cutting&) = delete;
        void operator=(const Cutting&) = delete;

    public:
        Cutting(Protocol* protocol, bool doIntegrityChecking);

    private:
        uint16 getWantedEmptyPrefix() const override;
        void input(Bytes&& msg) override;

    private:
        void pushChunk(Bytes&& chunkData, bool finalize);

    private:
        bool _doIntegrityChecking = true;

        /* CRC64 variant with "Jones" coefficients and init value of 0.
         *
         * Specification of this CRC64 variant follows:
         * Name: crc-64-jones
         * Width: 64 bites
         * Poly: 0xad93d23594c935a9
         * Reflected In: True
         * Xor_In: 0xffffffffffffffff
         * Reflected_Out: True
         * Xor_Out: 0x0
         * Check("123456789"): 0xe9c6d914c4b8d9ca
         */
        using Crc = boost::crc_optimal<64, 0xad93d23594c935a9, 0, 0, true, true>;
    };

    using CuttingPtr = std::unique_ptr<Cutting>;
}
