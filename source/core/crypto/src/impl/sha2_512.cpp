// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "sha2_512.hpp"
#include <dci/crypto/sha2_512.hpp>
#include <cstring>
#include <type_traits>
#include <dci/utils/endian.hpp>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    using namespace dci::utils::endian;

    namespace
    {
        static const std::size_t BLOCK_LENGTH           = 128;
        static const std::size_t SHORT_BLOCK_LENGTH     = (BLOCK_LENGTH - 8);

        inline std::uint64_t R(std::uint64_t  b, std::uint64_t x)
        {
            return (x >> b);
        }

        inline std::uint64_t S64(std::uint64_t b, std::uint64_t x)
        {
            return ((x >> b) | (x << (64 - b)));
        }

        inline std::uint64_t Ch(std::uint64_t  x, std::uint64_t y, std::uint64_t z)
        {
            return ((x & y) ^ ((~x) & z));
        }
        inline std::uint64_t Maj(std::uint64_t x, std::uint64_t y, std::uint64_t z)
        {
            return ((x & y) ^ (x & z) ^ (y & z));
        }


        inline std::uint64_t Sigma0_512(std::uint64_t x)
        {
            return (S64(28,  x) ^ S64(34, x) ^ S64(39, x));
        }

        inline std::uint64_t Sigma1_512(std::uint64_t x)
        {
            return (S64(14,  x) ^ S64(18, x) ^ S64(41, x));
        }

        inline std::uint64_t sigma0_512(std::uint64_t x)
        {
            return (S64(1 ,  x) ^ S64(8 , x) ^ R(7 ,   x));
        }

        inline std::uint64_t sigma1_512(std::uint64_t x)
        {
            return (S64(19, x) ^ S64(61, x) ^ R(6 ,   x));
        }

        const static std::array<std::uint64_t, 8> IV =
        {
            0x6a09e667f3bcc908ULL,
            0xbb67ae8584caa73bULL,
            0x3c6ef372fe94f82bULL,
            0xa54ff53a5f1d36f1ULL,
            0x510e527fade682d1ULL,
            0x9b05688c2b3e6c1fULL,
            0x1f83d9abfb41bd6bULL,
            0x5be0cd19137e2179ULL
        };

        const static std::uint64_t K512[80] =
        {
            0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
            0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
            0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
            0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
            0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
            0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
            0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
            0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
            0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
            0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
            0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
            0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
            0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
            0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
            0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
            0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_512::Sha2_512(std::size_t digestSize)
        : Hash{digestSize < 1 ? 1 : (digestSize > 64 ? 64 : digestSize)}
        , _state{IV}
        , _bitcount{}
        , _buffer{}
    {
        dbgAssert(digestSize >= 1);
        dbgAssert(digestSize <= 64);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_512::Sha2_512(const Sha2_512& from)
        : Hash{from}
        , _state{from._state}
        , _bitcount{from._bitcount}
        , _buffer{from._buffer}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_512::Sha2_512(Sha2_512&& from)
        : Hash{std::move(from)}
        , _state{from._state}
        , _bitcount{from._bitcount}
        , _buffer{from._buffer}
    {
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_512::~Sha2_512()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_512& Sha2_512::operator=(const Sha2_512& from)
    {
        static_cast<Hash&>(*this) = from;

        _state = from._state;
        _bitcount = from._bitcount;
        _buffer = from._buffer;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_512& Sha2_512::operator=(Sha2_512&& from)
    {
        static_cast<Hash&>(*this) = std::move(from);

        _state = from._state;
        _bitcount = from._bitcount;
        _buffer = from._buffer;

        from.clear();

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Sha2_512::clone()
    {
        return HashPtr
        {
            new crypto::Sha2_512{himpl::impl2Face<crypto::Sha2_512>(*this)},
            [](crypto::Hash* p){delete static_cast<crypto::Sha2_512*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Sha2_512::blockSize()
    {
        return BLOCK_LENGTH;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_512::add(const void* vdata, std::size_t len)
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
    void Sha2_512::barrier()
    {
        std::array<uint8_t, 64> digest;

        {
            Sha2_512 clone = *this;
            clone._digestSize = digest.size();
            clone.finish(digest.data());
        }

        add(digest.data(), digest.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_512::finish(void* digest)
    {
        finish(digest, _digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_512::finish(void* digest, std::size_t customDigestSize)
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
    void Sha2_512::clear()
    {
        _state = IV;
        _bitcount = 0;
        _buffer = std::array<std::uint8_t, 128>{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_512::transform(const void* data)
    {
        return transform(static_cast<const std::uint64_t*>(data));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_512::transform(const std::uint64_t* data)
    {
        std::uint64_t a, b, c, d, e, f, g, h, s0, s1;
        std::uint64_t T1, T2, *W512;

        W512 = static_cast<std::uint64_t*>(static_cast<void*>(_buffer.data()));

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
            W512[j] = n2b(*data++);

            T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + W512[j];

            T2 = Sigma0_512(a) + Maj(a, b, c);
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
            s0 = W512[(j+1)&0x0f];
            s0 = sigma0_512(s0);
            s1 = W512[(j+14)&0x0f];
            s1 = sigma1_512(s1);

            T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + (W512[j&0x0f] += s1 + W512[(j+9)&0x0f] + s0);
            T2 = Sigma0_512(a) + Maj(a, b, c);
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
        while (j < 80);

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
