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
#include <dci/bytes/chunk.hpp>

namespace dci::bytes
{
    class Buffer : public mm::heap::Allocable<Buffer>
    {
    private:
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        void operator=(const Buffer&) = delete;
        void operator=(Buffer&&) = delete;

    public:
        Buffer(Chunk* firstChunk = nullptr, Chunk* lastChunk = nullptr, uint32 size = 0);
        ~Buffer();

        uint32 shareCounter() const;

        Chunk* firstChunk();
        Chunk* lastChunk();
        uint32 size() const;

        uint32 detachData(Chunk*& firstChunk, Chunk*& lastChunk);

        void setFirstChunk(Chunk* chunk);
        void setLastChunk(Chunk* chunk);
        void addSize(uint32 size);
        void decSize(uint32 size);

    public:
        bool consistent() const;

    private:
        Chunk*  _firstChunk = nullptr;
        Chunk*  _lastChunk = nullptr;
        uint32  _size = 0;

        friend void intrusive_ptr_add_ref(Buffer*);
        friend void intrusive_ptr_release(Buffer*);
        uint32  _shareCounter = 0;
    };

    void intrusive_ptr_add_ref(Buffer* p);
    void intrusive_ptr_release(Buffer* p);
    using BufferPtr = boost::intrusive_ptr<Buffer>;

}
