// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../base.hpp"

namespace dci::module::stiac::stages::in
{
    class Cutting
        : public Base
    {
        Cutting(const Cutting&) = delete;
        void operator=(const Cutting&) = delete;

    public:
        Cutting(Protocol* protocol, bool doIntegrityChecking);

    private:
        void input(Bytes&& msg) override;

    private:
        bool _doIntegrityChecking = true;

        using Crc = boost::crc_optimal<64, 0xad93d23594c935a9, 0, 0, true, true>;

    private:
        Bytes   _input;
        uint32  _chunkSize = 0;
        bool    _chunkFinal = false;
        Bytes   _message;
    };

    using CuttingPtr = std::unique_ptr<Cutting>;
}
