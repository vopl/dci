/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "bytes.hpp"

namespace dci::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(const Bytes& from)
        : Container(from)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(Bytes&& from)
        : Container(static_cast<Container&&>(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(bytes::Chunk* first, bytes::Chunk* last, uint32 size)
    {
        begin().write(first, last, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(const void* data, uint32 size)
    {
        begin().write(data, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::Bytes(const char* csz)
    {
        begin().write(csz);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes::~Bytes()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes& Bytes::operator=(const Bytes& from)
    {
        Container::operator=(from);
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Bytes& Bytes::operator=(Bytes&& from)
    {
        Container::operator=(static_cast<Container&&>(from));
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator==(const Bytes& with) const
    {
        return compare(with) == 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator!=(const Bytes& with) const
    {
        return compare(with) != 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator<(const Bytes& with) const
    {
        return compare(with) < 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator>(const Bytes& with) const
    {
        return compare(with) > 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator<=(const Bytes& with) const
    {
        return compare(with) <= 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::operator>=(const Bytes& with) const
    {
        return compare(with) >= 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Bytes::operator<=>(const Bytes& with) const
    {
        return compare(with);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Bytes::empty() const
    {
        return !buffer() || !buffer()->size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Bytes::size() const
    {
        if(buffer())
        {
            return buffer()->size();
        }

        return 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Bytes::toString() const
    {
        return cbegin().toString();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Bytes::toHex() const
    {
        return cbegin().toHex();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Bytes::clear()
    {
        resetUsers();
        resetBuffer();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter Bytes::begin()
    {
        return Alter(this, true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor Bytes::begin() const
    {
        return Cursor(const_cast<Bytes*>(this), true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor Bytes::cbegin() const
    {
        return Cursor(const_cast<Bytes*>(this), true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Alter Bytes::end()
    {
        return Alter(this, false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor Bytes::end() const
    {
        return Cursor(const_cast<Bytes*>(this), false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor Bytes::cend() const
    {
        return Cursor(const_cast<Bytes*>(this), false);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Bytes::compare(const Bytes& with) const
    {
        if(this == &with)
        {
            return std::strong_ordering::equal;
        }

        Cursor cursor1{cbegin()};
        Cursor cursor2{with.cbegin()};

        return cursor1.compare(cursor2);
    }
}
