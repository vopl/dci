// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../user.hpp"
#include <compare>

namespace dci::impl
{
    class Bytes;
}

namespace dci::bytes::impl
{
    using Bytes = dci::impl::Bytes;
    class Alter;

    class Cursor
        : public User
    {
    public:
        static void tryDestruction(Cursor*);

    public:
        Cursor();
        Cursor(const Cursor& from);
        Cursor(Cursor&& from);
        Cursor(Bytes* container, bool posAtBegin);
        ~Cursor() override;

        Cursor& operator=(const Cursor& from);
        Cursor& operator=(Cursor&& from);

    public:
        bool atBegin() const;
        bool atEnd() const;

        uint32 pos() const;

        uint32 size() const;
        uint32 sizeBack() const;

        int32 advance(int32 offset);
        int32 advanceChunks(int32 amount);

        const byte* continuousData() const;
        uint32 continuousDataSize() const;
        uint32 continuousDataOffset() const;

        uint32 read(void* dst, uint32 maxSize);
        uint32 read(Bytes& dst, uint32 maxSize);
        uint32 read(Alter& dst, uint32 maxSize);

        std::strong_ordering compare(const void* with, uint32 size);
        std::strong_ordering compare(const Bytes& with);
        std::strong_ordering compare(Cursor& with);

        String toString(uint32 maxSize = ~uint32());
        String toHex(uint32 maxSize = ~uint32());

    protected:
        uint32 readImpl(auto&& with, uint32 maxSize);
        std::strong_ordering compareImpl(auto&& with);
        bool consistent() const;
    };
}
