// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../bytes/impl/alter.hpp"
#include "../bytes/container.hpp"

namespace dci::impl
{
    using Cursor = bytes::impl::Cursor;
    using Alter = bytes::impl::Alter;

    class Bytes final
        : public bytes::Container
    {
    public:
        Bytes();
        Bytes(const Bytes& from);
        Bytes(Bytes&& from);
        Bytes(bytes::Chunk* first, bytes::Chunk* last, uint32 size);
        Bytes(const void* data, uint32 size);
        Bytes(const char* csz);

        ~Bytes();

        Bytes& operator=(const Bytes& from);
        Bytes& operator=(Bytes&& from);

        bool operator==(const Bytes& with) const;
        bool operator!=(const Bytes& with) const;
        bool operator<(const Bytes& with) const;
        bool operator>(const Bytes& with) const;
        bool operator<=(const Bytes& with) const;
        bool operator>=(const Bytes& with) const;
        std::strong_ordering operator<=>(const Bytes& with) const;

        bool empty() const;
        uint32 size() const;

        String toString() const;
        String toHex() const;

        void clear();

        Alter begin();
        Cursor begin() const;
        Cursor cbegin() const;

        Alter end();
        Cursor end() const;
        Cursor cend() const;

    private:
        std::strong_ordering compare(const Bytes& with) const;
    };
}
