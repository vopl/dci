/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
