// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
