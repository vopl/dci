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
#include "cursor.hpp"

namespace dci::bytes::impl
{
    class Alter final
        : public Cursor
    {
    public:
        Alter();
        Alter(const Alter& from);
        Alter(Alter&& from);
        Alter(Bytes* container, bool posAtBegin);
        ~Alter() override;

        Alter& operator=(const Alter& from);
        Alter& operator=(Alter&& from);

        void reset() override;

    public:

        void advance(int32 offset);

        void* continuousData4Write();

        byte* prepareWriteBuffer(/*out*/uint32& size);
        void commitWriteBuffer(uint32 size);

        void write(const void* src, uint32 size);
        uint32 write(const char* srcz);
        uint32 write(const Bytes& src, uint32 maxSize = ~uint32());
        uint32 write(Bytes&& src, uint32 maxSize = ~uint32());
        uint32 write(Cursor& src, uint32 maxSize = ~uint32());
        void write(Chunk* srcFirst, Chunk* srcLast, uint32 size);

        uint32 remove(uint32 maxSize = ~uint32());
        uint32 removeTo(void* dst, uint32 maxSize = ~uint32());
        uint32 removeTo(Bytes& dst, uint32 maxSize = ~uint32());
        uint32 removeTo(Alter& dst, uint32 maxSize = ~uint32());

    private:
        uint32 writeImpl(auto&& src);
        uint32 removeImpl(uint32 maxSize, auto&& utilizeRaw, auto&& utilizeChunks);

    private:
        bool consistent() const;
        void copyBufferBecauseOfWrite();

    private:
        std::unique_ptr<Chunk> _preparedWriteBuffer = nullptr;
    };
}
