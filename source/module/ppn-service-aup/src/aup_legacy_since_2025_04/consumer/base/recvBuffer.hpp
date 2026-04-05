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
