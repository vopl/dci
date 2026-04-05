/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "blake2b.hpp"
#include <dci/crypto/blake2b.hpp>
#include <dci/utils/endian.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    namespace
    {
        const std::array<uint64_t, Blake2b::IVU64COUNT> IV =
        {
           0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
           0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
           0x510e527fade682d1, 0x9b05688c2b3e6c1f,
           0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
        };

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        /**
        * Bit rotation right by a compile-time constant amount
        * @param input the input word
        * @return input rotated right by ROT bits
        */
        template<size_t ROT, typename T>
        inline T __attribute__((always_inline)) rotr(T input)
        {
            static_assert(ROT > 0 && ROT < 8*sizeof(T), "Invalid rotation constant");
            return static_cast<T>((input >> ROT) | (input << (8*sizeof(T) - ROT)));
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        inline void __attribute__((always_inline)) G(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d,
                      uint64_t M0, uint64_t M1)
        {
            a = a + b + M0;
            d = rotr<32>(d ^ a);
            c = c + d;
            b = rotr<24>(b ^ c);
            a = a + b + M1;
            d = rotr<16>(d ^ a);
            c = c + d;
            b = rotr<63>(b ^ c);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template<size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6, size_t i7,
                 size_t i8, size_t i9, size_t iA, size_t iB, size_t iC, size_t iD, size_t iE, size_t iF>
        inline void __attribute__((always_inline)) ROUND(uint64_t* v, const uint64_t* M)
        {
            G(v[ 0], v[ 4], v[ 8], v[12], M[i0], M[i1]);
            G(v[ 1], v[ 5], v[ 9], v[13], M[i2], M[i3]);
            G(v[ 2], v[ 6], v[10], v[14], M[i4], M[i5]);
            G(v[ 3], v[ 7], v[11], v[15], M[i6], M[i7]);
            G(v[ 0], v[ 5], v[10], v[15], M[i8], M[i9]);
            G(v[ 1], v[ 6], v[11], v[12], M[iA], M[iB]);
            G(v[ 2], v[ 7], v[ 8], v[13], M[iC], M[iD]);
            G(v[ 3], v[ 4], v[ 9], v[14], M[iE], M[iF]);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2b::Blake2b(std::size_t digestSize)
        : Hash{digestSize < 1 ? 1 : (digestSize > 64 ? 64 : digestSize)}
        , _buffer{}
        , _bufpos{0}
        , _H{IV}
        , _T{}
        , _F{}
    {
        dbgAssert(digestSize >= 1);
        dbgAssert(digestSize <= 64);
        _H[0] ^= 0x01010000 ^ static_cast<uint8_t>(_digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2b::Blake2b(const Blake2b& from)
        : Hash{from}
        , _buffer{from._buffer}
        , _bufpos{from._bufpos}
        , _H{from._H}
        , _T{from._T}
        , _F{from._F}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2b::Blake2b(Blake2b&& from)
        : Hash{std::move(from)}
        , _buffer{from._buffer}
        , _bufpos{from._bufpos}
        , _H{from._H}
        , _T{from._T}
        , _F{from._F}
    {
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2b::~Blake2b()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2b& Blake2b::operator=(const Blake2b& from)
    {
        static_cast<Hash&>(*this) = from;

        _buffer = from._buffer;
        _bufpos = from._bufpos;

        _H = from._H;
        _T = from._T;
        _F = from._F;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2b& Blake2b::operator=(Blake2b&& from)
    {
        static_cast<Hash&>(*this) = std::move(from);

        _buffer = from._buffer;
        _bufpos = from._bufpos;

        _H = from._H;
        _T = from._T;
        _F = from._F;

        from.clear();

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Blake2b::clone()
    {
        return HashPtr
        {
            new crypto::Blake2b{himpl::impl2Face<crypto::Blake2b>(*this)},
            [](crypto::Hash*p){delete static_cast<crypto::Blake2b*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake2b::blockSize()
    {
        return BLOCKBYTES;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2b::add(const void* vdata, std::size_t len)
    {
        if(!len)
        {
            return;
        }

        const uint8_t* input = static_cast<const uint8_t*>(vdata);

        if(_bufpos > 0)
        {
            if(_bufpos < BLOCKBYTES)
            {
                const size_t take = std::min(BLOCKBYTES - _bufpos, len);
                memcpy(&_buffer[_bufpos], input, take);
                _bufpos += take;
                len -= take;
                input += take;
            }

            if(_bufpos == _buffer.size() && len > 0)
            {
                compress(_buffer.data(), 1, BLOCKBYTES);
                _bufpos = 0;
            }
        }

        if(len > BLOCKBYTES)
        {
            const size_t full_blocks = ((len-1) / BLOCKBYTES);
            compress(input, full_blocks, BLOCKBYTES);

            input += full_blocks * BLOCKBYTES;
            len -= full_blocks * BLOCKBYTES;
        }

        if(len > 0)
        {
            memcpy(&_buffer[_bufpos], input, len);
            _bufpos += len;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2b::barrier()
    {
        std::array<uint8_t, 64> digest;

        {
            Blake2b clone = *this;
            clone._digestSize = digest.size();
            clone.finish(digest.data());
        }

        add(digest.data(), digest.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2b::finish(void* digest)
    {
        finish(digest, _digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2b::finish(void* digest, std::size_t customDigestSize)
    {
        if(_bufpos != BLOCKBYTES)
        {
            memset(&_buffer[_bufpos], 0, BLOCKBYTES - _bufpos);
        }

        _F[0] = 0xFFFFFFFFFFFFFFFF;
        compress(_buffer.data(), 1, _bufpos);

        if constexpr(std::endian::little != std::endian::native)
        {
            for(std::size_t j = 0; j < _H.size(); j++)
            {
                _H[j] = dci::utils::endian::n2l(_H[j]);
            }
        }

        memcpy(digest, _H.data(), std::min(customDigestSize, _digestSize));

        clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2b::clear()
    {
        _buffer = std::array<uint8_t, BLOCKBYTES>{};
        _bufpos = 0;
        _H = IV;
        _T = std::array<uint64_t, 2>{};
        _F = std::array<uint64_t, 2>{};

        _H[0] ^= 0x01010000 ^ static_cast<uint8_t>(_digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2b::compress(const uint8_t* input, size_t blocks, uint64_t increment)
    {
        for(size_t b = 0; b != blocks; ++b)
        {
            _T[0] += increment;
            if(_T[0] < increment)
            {
                _T[1]++;
            }

            uint64_t M[16];
            if constexpr(std::endian::little == std::endian::native)
            {
                memcpy(M, input, sizeof(M));
            }
            else
            {
                const uint64_t* input64 = static_cast<const uint64_t*>(static_cast<const void*>(input));
                for(size_t i(0); i<16; ++i)
                {
                    M[i] = dci::utils::endian::n2l(input64[i]);
                }
            }

            input += BLOCKBYTES;

            uint64_t v[16];

            for(size_t i = 0; i < 8; i++)
            {
                v[i] = _H[i];
            }

            for(size_t i = 0; i != 8; ++i)
            {
                v[i + 8] = IV[i];
            }

            v[12] ^= _T[0];
            v[13] ^= _T[1];
            v[14] ^= _F[0];
            v[15] ^= _F[1];

            ROUND< 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15>(v, M);
            ROUND<14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3>(v, M);
            ROUND<11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4>(v, M);
            ROUND< 7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8>(v, M);
            ROUND< 9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13>(v, M);
            ROUND< 2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9>(v, M);
            ROUND<12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11>(v, M);
            ROUND<13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10>(v, M);
            ROUND< 6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5>(v, M);
            ROUND<10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13,  0>(v, M);
            ROUND< 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15>(v, M);
            ROUND<14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3>(v, M);

            for(size_t i = 0; i < 8; i++)
            {
                _H[i] ^= v[i] ^ v[i + 8];
            }
        }
    }

}
