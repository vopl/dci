/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "sha2_256.hpp"
#include <dci/crypto/sha2_256.hpp>
#include <cstring>
#include <type_traits>
#include <dci/utils/endian.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    using namespace dci::utils::endian;

    namespace
    {
        static const std::size_t BLOCK_LENGTH           = 64;
        static const std::size_t SHORT_BLOCK_LENGTH     = (BLOCK_LENGTH - 8);

        inline std::uint32_t R(std::uint32_t  b, std::uint32_t x)
        {
            return (x >> b);
        }

        inline std::uint32_t S32(std::uint32_t b, std::uint32_t x)
        {
            return ((x >> b) | (x << (32 - b)));
        }

        inline std::uint32_t Ch(std::uint32_t  x, std::uint32_t y, std::uint32_t z)
        {
            return ((x & y) ^ ((~x) & z));
        }
        inline std::uint32_t Maj(std::uint32_t x, std::uint32_t y, std::uint32_t z)
        {
            return ((x & y) ^ (x & z) ^ (y & z));
        }


        inline std::uint32_t Sigma0_256(std::uint32_t x)
        {
            return (S32(2,  x) ^ S32(13, x) ^ S32(22, x));
        }

        inline std::uint32_t Sigma1_256(std::uint32_t x)
        {
            return (S32(6,  x) ^ S32(11, x) ^ S32(25, x));
        }

        inline std::uint32_t sigma0_256(std::uint32_t x)
        {
            return (S32(7,  x) ^ S32(18, x) ^ R(3 ,   x));
        }

        inline std::uint32_t sigma1_256(std::uint32_t x)
        {
            return (S32(17, x) ^ S32(19, x) ^ R(10,   x));
        }

        const static std::array<std::uint32_t, 8> IV =
        {
            0x6a09e667UL,
            0xbb67ae85UL,
            0x3c6ef372UL,
            0xa54ff53aUL,
            0x510e527fUL,
            0x9b05688cUL,
            0x1f83d9abUL,
            0x5be0cd19UL
        };

        const static std::uint32_t K256[64] =
        {
            0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
            0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
            0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
            0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
            0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
            0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
            0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
            0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
            0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
            0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
            0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
            0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
            0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
            0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
            0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
            0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::Sha2_256(std::size_t digestSize)
        : Hash{digestSize < 1 ? 1 : (digestSize > 32 ? 32 : digestSize)}
        , _state{IV}
        , _bitcount{}
        , _buffer{}
    {
        dbgAssert(digestSize >= 1);
        dbgAssert(digestSize <= 32);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::Sha2_256(const Sha2_256& from)
        : Hash{from}
        , _state{from._state}
        , _bitcount{from._bitcount}
        , _buffer{from._buffer}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::Sha2_256(Sha2_256&& from)
        : Hash{std::move(from)}
        , _state{from._state}
        , _bitcount{from._bitcount}
        , _buffer{from._buffer}
    {
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::~Sha2_256()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256& Sha2_256::operator=(const Sha2_256& from)
    {
        static_cast<Hash&>(*this) = from;

        _state = from._state;
        _bitcount = from._bitcount;
        _buffer = from._buffer;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256& Sha2_256::operator=(Sha2_256&& from)
    {
        static_cast<Hash&>(*this) = std::move(from);

        _state = from._state;
        _bitcount = from._bitcount;
        _buffer = from._buffer;

        from.clear();

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Sha2_256::clone()
    {
        return HashPtr
        {
            new crypto::Sha2_256{himpl::impl2Face<crypto::Sha2_256>(*this)},
            [](crypto::Hash* p){delete static_cast<crypto::Sha2_256*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Sha2_256::blockSize()
    {
        return BLOCK_LENGTH;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::add(const void* vdata, std::size_t len)
    {
        unsigned int freespace, usedspace;

        if(len == 0)
        {
            return;
        }

        const char* data = static_cast<const char*>(vdata);

        usedspace = (_bitcount >> 3) % BLOCK_LENGTH;

        if(usedspace > 0)
        {
            freespace = BLOCK_LENGTH - usedspace;

            if(len >= freespace)
            {
                memcpy(&_buffer[usedspace], data, freespace);
                _bitcount += freespace << 3;
                len -= freespace;
                data += freespace;
                transform(_buffer.data());
            }
            else
            {
                memcpy(&_buffer[usedspace], data, len);
                _bitcount += len << 3;
                return;
            }
        }
        while (len >= BLOCK_LENGTH)
        {
            transform(data);
            _bitcount += BLOCK_LENGTH << 3;
            len -= BLOCK_LENGTH;
            data += BLOCK_LENGTH;
        }
        if(len > 0)
        {
            memcpy(_buffer.data(), data, len);
            _bitcount += len << 3;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::barrier()
    {
        std::array<uint8_t, 32> digest;

        {
            Sha2_256 clone = *this;
            clone._digestSize = digest.size();
            clone.finish(digest.data());
        }

        add(digest.data(), digest.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::finish(void* digest)
    {
        finish(digest, _digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::finish(void* digest, std::size_t customDigestSize)
    {
        unsigned int usedspace = (_bitcount >> 3) % BLOCK_LENGTH;

        _bitcount = n2b(_bitcount);

        if(usedspace > 0)
        {
            _buffer[usedspace++] = 0x80;

            if(usedspace <= SHORT_BLOCK_LENGTH)
            {
                memset(&_buffer[usedspace], 0, SHORT_BLOCK_LENGTH - usedspace);
            }
            else
            {
                if(usedspace < BLOCK_LENGTH)
                {
                    memset(&_buffer[usedspace], 0, BLOCK_LENGTH - usedspace);
                }
                transform(_buffer.data());

                memset(_buffer.data(), 0, SHORT_BLOCK_LENGTH);
            }
        }
        else
        {
            memset(_buffer.data(), 0, SHORT_BLOCK_LENGTH);

            _buffer[0] = 0x80;
        }

        void* bcPtr = &_buffer[SHORT_BLOCK_LENGTH];
        *static_cast<std::uint64_t*>(bcPtr) = _bitcount;

        transform(_buffer.data());

        if constexpr(std::endian::big != std::endian::native)
        {
            for(std::size_t j = 0; j < 8; j++)
            {
                _state[j] = n2b(_state[j]);
            }
        }

        memcpy(digest, _state.data(), std::min(customDigestSize, _digestSize));
        clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::clear()
    {
        _state = IV;
        _bitcount = 0;
        _buffer = std::array<std::uint8_t, 64>{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::transform(const void* data)
    {
        return transform(static_cast<const std::uint32_t*>(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::transform(const std::uint32_t* data)
    {
        std::uint32_t a, b, c, d, e, f, g, h, s0, s1;
        std::uint32_t T1, T2, *W256;

        W256 = static_cast<std::uint32_t*>(static_cast<void*>(_buffer.data()));

        a = _state[0];
        b = _state[1];
        c = _state[2];
        d = _state[3];
        e = _state[4];
        f = _state[5];
        g = _state[6];
        h = _state[7];

        int j = 0;
        do
        {
            W256[j] = n2b(*data++);

            T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];

            T2 = Sigma0_256(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;

            j++;
        }
        while (j < 16);

        do
        {
            s0 = W256[(j+1)&0x0f];
            s0 = sigma0_256(s0);
            s1 = W256[(j+14)&0x0f];
            s1 = sigma1_256(s1);

            T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);
            T2 = Sigma0_256(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;

            j++;
        }
        while (j < 64);

        _state[0] += a;
        _state[1] += b;
        _state[2] += c;
        _state[3] += d;
        _state[4] += e;
        _state[5] += f;
        _state[6] += g;
        _state[7] += h;
    }
}
