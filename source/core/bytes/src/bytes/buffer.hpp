// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
