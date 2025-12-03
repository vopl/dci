// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "blake2s.hpp"
#include <dci/crypto/blake2s.hpp>
#include <dci/utils/endian.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    namespace
    {
        const std::array<uint32_t, Blake2s::IVU32COUNT> IV =
        {
           0x6a09e667, 0xbb67ae85,
           0x3c6ef372, 0xa54ff53a,
           0x510e527f, 0x9b05688c,
           0x1f83d9ab, 0x5be0cd19
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
        inline void __attribute__((always_inline)) G(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
                      uint32_t M0, uint32_t M1)
        {
            a = a + b + M0;
            d = rotr<16>(d ^ a);
            c = c + d;
            b = rotr<12>(b ^ c);
            a = a + b + M1;
            d = rotr<8>(d ^ a);
            c = c + d;
            b = rotr<7>(b ^ c);
        }

        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        template<size_t i0, size_t i1, size_t i2, size_t i3, size_t i4, size_t i5, size_t i6, size_t i7,
                 size_t i8, size_t i9, size_t iA, size_t iB, size_t iC, size_t iD, size_t iE, size_t iF>
        inline void __attribute__((always_inline)) ROUND(uint32_t* v, const uint32_t* M)
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
    Blake2s::Blake2s(std::size_t digestSize)
        : Hash{digestSize < 1 ? 1 : (digestSize > 32 ? 32 : digestSize)}
        , _buffer{}
        , _bufpos{0}
        , _H{IV}
        , _T{}
        , _F{}
    {
        dbgAssert(digestSize >= 1);
        dbgAssert(digestSize <= 32);
        _H[0] ^= 0x01010000 ^ static_cast<uint8_t>(_digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::Blake2s(const Blake2s& from)
        : Hash{from}
        , _buffer(from._buffer)
        , _bufpos(from._bufpos)
        , _H(from._H)
        , _T(from._T)
        , _F(from._F)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::Blake2s(Blake2s&& from)
        : Hash{std::move(from)}
        , _buffer(from._buffer)
        , _bufpos(from._bufpos)
        , _H(from._H)
        , _T(from._T)
        , _F(from._F)
    {
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s::~Blake2s()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake2s& Blake2s::operator=(const Blake2s& from)
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
    Blake2s& Blake2s::operator=(Blake2s&& from)
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
    HashPtr Blake2s::clone()
    {
        return HashPtr
        {
            new crypto::Blake2s{himpl::impl2Face<crypto::Blake2s>(*this)},
            [](crypto::Hash*p){delete static_cast<crypto::Blake2s*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake2s::blockSize()
    {
        return BLOCKBYTES;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::add(const void* vdata, std::size_t len)
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
    void Blake2s::barrier()
    {
        std::array<uint8_t, 32> digest;

        {
            Blake2s clone = *this;
            clone._digestSize = digest.size();
            clone.finish(digest.data());
        }

        add(digest.data(), digest.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::finish(void* digest)
    {
        finish(digest, _digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::finish(void* digest, std::size_t customDigestSize)
    {
        if(_bufpos != BLOCKBYTES)
        {
            memset(&_buffer[_bufpos], 0, BLOCKBYTES - _bufpos);
        }

        _F[0] = 0xFFFFFFFF;
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
    void Blake2s::clear()
    {
        _buffer = std::array<uint8_t, BLOCKBYTES>{};
        _bufpos = 0;
        _H = IV;
        _T = std::array<uint32_t, 2>{};
        _F = std::array<uint32_t, 2>{};

        _H[0] ^= 0x01010000 ^ static_cast<uint8_t>(_digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake2s::compress(const uint8_t* input, size_t blocks, uint64_t increment)
    {
        for(size_t b = 0; b != blocks; ++b)
        {
            _T[0] += increment;
            if(_T[0] < increment)
            {
                _T[1]++;
            }

            uint32_t M[16];
            if constexpr(std::endian::little == std::endian::native)
            {
                memcpy(M, input, sizeof(M));
            }
            else
            {
                const uint32_t* input32 = static_cast<const uint32_t*>(static_cast<const void*>(input));
                for(size_t i(0); i<16; ++i)
                {
                    M[i] = dci::utils::endian::n2l(input32[i]);
                }
            }

            input += BLOCKBYTES;

            uint32_t v[16];

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

            for(size_t i = 0; i < 8; i++)
            {
                _H[i] ^= v[i] ^ v[i + 8];
            }
        }
    }

}
