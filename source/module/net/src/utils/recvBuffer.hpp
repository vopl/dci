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

namespace dci::module::net::utils
{
    class RecvBuffer
    {
        RecvBuffer(const RecvBuffer&) = delete;
        void operator=(const RecvBuffer&) = delete;

    public:
        RecvBuffer();
        ~RecvBuffer();

        void limitDataSize(uint32 maxDataSize);
        void unlimitDataSize();

        Buf* bufs();
        uint32 bufsAmount();
        uint32 bufSize();
        uint32 dataSize();

        Bytes detach(uint32 size);

    private:
        void renewFront(uint32 bufsAmount);

    private:
        static constexpr uint32 _bufSize = bytes::Chunk::bufferSize();
        static constexpr uint32 _maxBufsAmount = Buf::_maxBufs;
        static constexpr uint32 _maxDataSize = _bufSize * _maxBufsAmount;

    private:
        bytes::Chunk*   _chunks[_maxBufsAmount];
        Buf             _bufs[_maxBufsAmount];

        uint32          _bufsAmount{_maxBufsAmount};
        uint32          _dataSize{_maxDataSize};
    };
}
