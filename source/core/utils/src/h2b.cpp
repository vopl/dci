/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include <dci/utils/h2b.hpp>
#include <cstdint>

namespace dci::utils
{
    namespace
    {
        bool dgt(char h, uint8_t& b)
        {
            if(h >= '0' && h <= '9')
            {
                b = static_cast<uint8_t>(h-'0');
                return true;
            }

            if(h >= 'a' && h <= 'f')
            {
                b = static_cast<uint8_t>(h-'a'+10);
                return true;
            }

            if(h >= 'A' && h <= 'F')
            {
                b = static_cast<uint8_t>(h-'A'+10);
                return true;
            }

            return false;
        }

        bool h2b_little(const char* h, std::size_t hsize, std::uint8_t* b)
        {
            std::size_t i{0};
            for(; i<hsize/2*2; i+=2, ++b)
            {
                uint8_t hi, lo;
                if(!dgt(h[i+0], lo)) return false;
                if(!dgt(h[i+1], hi)) return false;

                *b = static_cast<uint8_t>((hi<<4) | lo);
            }

            if(hsize & 1)
            {
                uint8_t lo;
                if(!dgt(h[i+0], lo)) return false;

                *b = lo;
            }

            return true;
        }

        bool h2b_middle(const char* h, std::size_t hsize, std::uint8_t* b)
        {
            std::size_t i{0};
            for(; i<hsize/2*2; i+=2, ++b)
            {
                uint8_t hi, lo;
                if(!dgt(h[i+0], hi)) return false;
                if(!dgt(h[i+1], lo)) return false;

                *b = static_cast<uint8_t>((hi<<4) | lo);
            }

            if(hsize & 1)
            {
                uint8_t hi;
                if(!dgt(h[i+0], hi)) return false;

                *b = static_cast<uint8_t>(hi<<4);
            }

            return true;
        }

        bool h2b_big(const char* h, std::size_t hsize, std::uint8_t* b)
        {
            if(hsize & 1)
            {
                uint8_t lo;
                if(!dgt(h[hsize-1], lo)) return false;

                *b = lo;
                ++b;
                hsize -= 1;
            }

            std::size_t i{0};
            for(; i<hsize; i+=2, ++b)
            {
                uint8_t hi, lo;
                if(!dgt(h[hsize-1-(i+0)], lo)) return false;
                if(!dgt(h[hsize-1-(i+1)], hi)) return false;

                *b = static_cast<uint8_t>((hi<<4) | lo);
            }

            return true;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool h2b(const void* h, std::size_t hsize, void* b, HexEndian he)
    {
        const char* h_ = static_cast<const char *>(h);
        std::uint8_t* b_ = static_cast<std::uint8_t *>(b);

        switch(he)
        {
        case HexEndian::little:
            return h2b_little(h_, hsize, b_);
        case HexEndian::middle:
            return h2b_middle(h_, hsize, b_);
        case HexEndian::big:
            return h2b_big(h_, hsize, b_);
        }

        return false;
    }
}
