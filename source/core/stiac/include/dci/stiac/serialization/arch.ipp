/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "arch.hpp"
#include "../serialization.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Arch::Arch(const bytes::Alter& from)
        : bytes::Alter(from)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Arch::Arch(bytes::Alter&& from)
        : bytes::Alter(std::move(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Arch::read(void* data, uint32 size)
    {
        if(size != bytes::Alter::read(data, size))
        {
            fail("low data");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void Arch::read(Bytes& data, uint32 size)
    {
        if(size != bytes::Alter::read(data, size))
        {
            fail("low data");
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    [[noreturn]] inline void Arch::fail(const char* cszDetails)
    {
        throw std::runtime_error(cszDetails);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Arch& Arch::operator<<(auto&& v)
    {
        using ::dci::stiac::serialization::save;

        save(*this, std::forward<decltype(v)>(v));
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Arch& Arch::operator>>(auto&& v)
    {
        using ::dci::stiac::serialization::load;

        load(*this, std::forward<decltype(v)>(v));
        return *this;
    }

}
