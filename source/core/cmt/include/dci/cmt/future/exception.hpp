/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/exception.hpp>

namespace dci::cmt::future
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Exception
        : public dci::exception::Skeleton<Exception, dci::Exception>
    {
    public:
        using dci::exception::Skeleton<Exception, dci::Exception>::Skeleton;

    public:
        static constexpr Eid _eid {0x10,0xd0,0x3b,0x22,0x01,0x57,0x44,0xf0,0x92,0x7a,0x5d,0xd1,0xdf,0xce,0x15,0x80};
    };

    namespace exception
    {
        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class Unresolved
            : public dci::exception::Skeleton<Unresolved, Exception>
        {
        public:
            using dci::exception::Skeleton<Unresolved, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x4b,0xf1,0xeb,0x4b,0x44,0x36,0x43,0x79,0xbe,0xcc,0xdf,0xd4,0xfc,0x77,0x1e,0x43};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class Canceled
            : public dci::exception::Skeleton<Canceled, Exception>
        {
        public:
            using dci::exception::Skeleton<Canceled, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x9e,0x5b,0x49,0xc7,0xc3,0xb4,0x41,0x3d,0xb0,0x26,0x1a,0x3c,0x9c,0xb7,0xc7,0xb5};
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        class ResolvedToValue
            : public dci::exception::Skeleton<ResolvedToValue, Exception>
        {
        public:
            using dci::exception::Skeleton<ResolvedToValue, Exception>::Skeleton;

        public:
            static constexpr Eid _eid {0x6e,0x96,0xde,0xf9,0xc4,0x77,0x43,0x42,0xaa,0xda,0xec,0xe5,0x7f,0x9d,0x83,0xd3};
        };
    }
}
