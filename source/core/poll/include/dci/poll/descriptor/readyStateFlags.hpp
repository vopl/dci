/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

namespace dci::poll::descriptor
{
    enum ReadyStateFlags : unsigned
    {
        rsf_null    = 0x000,

        rsf_read    = 0x001,
        rsf_pri     = 0x002,
        rsf_write   = 0x004,

        rsf_error   = 0x010,

        rsf_eof     = 0x100,
        rsf_close   = 0x200,
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags operator~(ReadyStateFlags a)
    {
        return static_cast<ReadyStateFlags>(~static_cast<unsigned>(a));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags operator&(ReadyStateFlags a, ReadyStateFlags b)
    {
        return static_cast<ReadyStateFlags>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags operator|(ReadyStateFlags a, ReadyStateFlags b)
    {
        return static_cast<ReadyStateFlags>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags& operator&=(ReadyStateFlags& a, ReadyStateFlags b)
    {
        a = a & b;
        return a;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    constexpr ReadyStateFlags& operator|=(ReadyStateFlags& a, ReadyStateFlags b)
    {
        a = a | b;
        return a;
    }
}
