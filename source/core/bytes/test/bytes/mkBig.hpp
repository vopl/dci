/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/bytes.hpp>

using namespace dci;
using namespace dci::bytes;

namespace
{
    Bytes mkBig()
    {
        Chunk* first = new Chunk;
        Chunk* last = first;
        uint32 size = 0;

        const uint16 sbegins[] = {0,1,2,3,0,9,8,7,6,0,0,1,7,3,356,23,0,1,97,0,1,0,1};
        const uint16 ssizes[]  = {1,1,2,3,3,1,5,6,7,7,1,1,7,9,123,43,1,2,45,2,2,1,1};

        for(uint32 si(0); si < sizeof(sbegins)/sizeof(sbegins[0]); ++si)
        {
            uint16 sb = sbegins[si];
            uint16 ss = ssizes[si];

            last->setBeginEnd(sb, sb+ss);

            for(uint32 i(0); i<ss; ++i)
            {
                last->data()[i] = byte(size+i);
            }

            size += ss;
            last->setNext(new Chunk{nullptr, last});
            last = last->next();
        }

        uint16 ss = 10;
        last->setEnd(ss);

        for(uint32 i(0); i<ss; ++i)
        {
            last->data()[i] = byte(size+i);
        }
        size += ss;

        return Bytes(first, last, size);
    }
}
