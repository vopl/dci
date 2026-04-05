/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "impl/cursor.hpp"
#include <compare>
#include <dci/bytes/cursor.hpp>
#include <dci/bytes.hpp>

namespace dci::bytes
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(himpl::FakeConstructionArg fca)
        : himpl::FaceLayout<Cursor, impl::Cursor>(fca)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor()
        : himpl::FaceLayout<Cursor, impl::Cursor>()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(const Cursor& from)
        : himpl::FaceLayout<Cursor, impl::Cursor>(himpl::face2Impl(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::Cursor(Cursor&& from)
        : himpl::FaceLayout<Cursor, impl::Cursor>(himpl::face2Impl(std::move(from)))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor::~Cursor()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor& Cursor::operator=(const Cursor& from)
    {
        return himpl::impl2Face<Cursor>(impl()=himpl::face2Impl(from));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Cursor& Cursor::operator=(Cursor&& from)
    {
        return himpl::impl2Face<Cursor>(impl()=himpl::face2Impl(std::move(from)));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Cursor::atBegin() const
    {
        return impl().atBegin();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Cursor::atEnd() const
    {
        return impl().atEnd();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::pos() const
    {
        return impl().pos();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::size() const
    {
        return impl().size();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::sizeBack() const
    {
        return impl().sizeBack();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int32 Cursor::advance(int32 offset)
    {
        return impl().advance(offset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int32 Cursor::advanceChunks(int32 amount)
    {
        return impl().advanceChunks(amount);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const byte* Cursor::continuousData() const
    {
        return impl().continuousData();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::continuousDataSize() const
    {
        return impl().continuousDataSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::continuousDataOffset() const
    {
        return impl().continuousDataOffset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::read(void* dst, uint32 maxSize)
    {
        return impl().read(dst, maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::read(Bytes& dst, uint32 maxSize)
    {
        return impl().read(himpl::face2Impl(dst), maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    uint32 Cursor::read(Alter& dst, uint32 maxSize)
    {
        return impl().read(himpl::face2Impl(dst), maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compare(const void* with, uint32 size)
    {
        return impl().compare(with, size);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compare(const Bytes& with)
    {
        return impl().compare(himpl::face2Impl(with));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::strong_ordering Cursor::compare(Cursor& with)
    {
        return impl().compare(himpl::face2Impl(with));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Cursor::toString(uint32 maxSize)
    {
        return impl().toString(maxSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    String Cursor::toHex(uint32 maxSize)
    {
        return impl().toHex(maxSize);
    }

}
