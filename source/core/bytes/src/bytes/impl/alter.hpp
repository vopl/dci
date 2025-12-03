// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
