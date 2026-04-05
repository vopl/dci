/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "chaCha.hpp"
#include <dci/utils/endian.hpp>
#include <dci/utils/dbg.hpp>
#include <cstring>

#define CHACHA_QUARTER_ROUND(a, b, c, d) \
      do {                               \
      a += b; d ^= a; d = rotl<16>(d);   \
      c += d; b ^= c; b = rotl<12>(b);   \
      a += b; d ^= a; d = rotl<8>(d);    \
      c += d; b ^= c; b = rotl<7>(b);    \
      } while(0)


namespace dci::crypto::impl
{
    namespace
    {
        template<size_t ROT, typename T>
        inline constexpr T rotl(T input)
        {
            static_assert(ROT > 0 && ROT < 8*sizeof(T), "Invalid rotation constant");
            return static_cast<T>((input << ROT) | (input >> (8*sizeof(T) - ROT)));
        }

        void hchacha(uint32_t output[8], const uint32_t input[16], size_t rounds)
        {
            dbgAssert(rounds % 2 == 0);

            uint32_t x00 = input[ 0], x01 = input[ 1], x02 = input[ 2], x03 = input[ 3],
                     x04 = input[ 4], x05 = input[ 5], x06 = input[ 6], x07 = input[ 7],
                     x08 = input[ 8], x09 = input[ 9], x10 = input[10], x11 = input[11],
                     x12 = input[12], x13 = input[13], x14 = input[14], x15 = input[15];

            for(size_t i = 0; i != rounds / 2; ++i)
            {
                CHACHA_QUARTER_ROUND(x00, x04, x08, x12);
                CHACHA_QUARTER_ROUND(x01, x05, x09, x13);
                CHACHA_QUARTER_ROUND(x02, x06, x10, x14);
                CHACHA_QUARTER_ROUND(x03, x07, x11, x15);

                CHACHA_QUARTER_ROUND(x00, x05, x10, x15);
                CHACHA_QUARTER_ROUND(x01, x06, x11, x12);
                CHACHA_QUARTER_ROUND(x02, x07, x08, x13);
                CHACHA_QUARTER_ROUND(x03, x04, x09, x14);
            }

            output[0] = x00;
            output[1] = x01;
            output[2] = x02;
            output[3] = x03;
            output[4] = x12;
            output[5] = x13;
            output[6] = x14;
            output[7] = x15;
        }

        void chacha_x8(uint8_t output[64*8], uint32_t input[16], size_t rounds)
        {
            dbgAssert(rounds % 2 == 0);

            uint32_t* output4 = static_cast<uint32_t*>(static_cast<void*>(output));

            // TODO interleave rounds
            for(size_t i = 0; i != 8; ++i)
            {
                uint32_t x00 = input[ 0], x01 = input[ 1], x02 = input[ 2], x03 = input[ 3],
                         x04 = input[ 4], x05 = input[ 5], x06 = input[ 6], x07 = input[ 7],
                         x08 = input[ 8], x09 = input[ 9], x10 = input[10], x11 = input[11],
                         x12 = input[12], x13 = input[13], x14 = input[14], x15 = input[15];

                for(size_t r = 0; r != rounds / 2; ++r)
                {
                    CHACHA_QUARTER_ROUND(x00, x04, x08, x12);
                    CHACHA_QUARTER_ROUND(x01, x05, x09, x13);
                    CHACHA_QUARTER_ROUND(x02, x06, x10, x14);
                    CHACHA_QUARTER_ROUND(x03, x07, x11, x15);

                    CHACHA_QUARTER_ROUND(x00, x05, x10, x15);
                    CHACHA_QUARTER_ROUND(x01, x06, x11, x12);
                    CHACHA_QUARTER_ROUND(x02, x07, x08, x13);
                    CHACHA_QUARTER_ROUND(x03, x04, x09, x14);
                }

                x00 += input[0];
                x01 += input[1];
                x02 += input[2];
                x03 += input[3];
                x04 += input[4];
                x05 += input[5];
                x06 += input[6];
                x07 += input[7];
                x08 += input[8];
                x09 += input[9];
                x10 += input[10];
                x11 += input[11];
                x12 += input[12];
                x13 += input[13];
                x14 += input[14];
                x15 += input[15];

                output4[16*i +  0] = dci::utils::endian::n2l(x00);
                output4[16*i +  1] = dci::utils::endian::n2l(x01);
                output4[16*i +  2] = dci::utils::endian::n2l(x02);
                output4[16*i +  3] = dci::utils::endian::n2l(x03);
                output4[16*i +  4] = dci::utils::endian::n2l(x04);
                output4[16*i +  5] = dci::utils::endian::n2l(x05);
                output4[16*i +  6] = dci::utils::endian::n2l(x06);
                output4[16*i +  7] = dci::utils::endian::n2l(x07);
                output4[16*i +  8] = dci::utils::endian::n2l(x08);
                output4[16*i +  9] = dci::utils::endian::n2l(x09);
                output4[16*i + 10] = dci::utils::endian::n2l(x10);
                output4[16*i + 11] = dci::utils::endian::n2l(x11);
                output4[16*i + 12] = dci::utils::endian::n2l(x12);
                output4[16*i + 13] = dci::utils::endian::n2l(x13);
                output4[16*i + 14] = dci::utils::endian::n2l(x14);
                output4[16*i + 15] = dci::utils::endian::n2l(x15);

                input[12]++;
                input[13] += (input[12] == 0);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::ChaCha(std::size_t rounds)
        : StreamCipher{}
        , _rounds{rounds}
        , _key{std::array<uint32_t, 8>{}}
        , _keySize{0}
        , _state{std::array<uint32_t, 16>{}}
        , _buffer{std::array<uint8_t, 8*64>{}}
        , _position{0}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::ChaCha(const ChaCha& from)
        : StreamCipher{from}
        , _rounds{from._rounds}
        , _key{from._key}
        , _keySize{from._keySize}
        , _state{from._state}
        , _buffer{from._buffer}
        , _position{from._position}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::ChaCha(ChaCha&& from)
        : StreamCipher{std::move(from)}
        , _rounds{from._rounds}
        , _key{from._key}
        , _keySize{from._keySize}
        , _state{from._state}
        , _buffer{from._buffer}
        , _position{from._position}
    {
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::~ChaCha()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha& ChaCha::operator=(const ChaCha& from)
    {
        static_cast<StreamCipher&>(*this) = from;

        _rounds = from._rounds;
        _key = from._key;
        _keySize = from._keySize;
        _state = from._state;
        _buffer = from._buffer;
        _position = from._position;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha& ChaCha::operator=(ChaCha&& from)
    {
        static_cast<StreamCipher&>(*this) = from;

        _rounds = from._rounds;
        _key = from._key;
        _keySize = from._keySize;
        _state = from._state;
        _buffer = from._buffer;
        _position = from._position;

        from.clear();

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::setKey(const void* key, std::size_t len)
    {
        _keySize = len/4;
        _key = std::array<uint32_t, 8>{};
        memcpy(_key.data(), key, std::min(len, std::size_t{32}));
        _key[0] = dci::utils::endian::n2l(_key[0]);
        _key[1] = dci::utils::endian::n2l(_key[1]);
        _key[2] = dci::utils::endian::n2l(_key[2]);
        _key[3] = dci::utils::endian::n2l(_key[3]);

        setIv(nullptr, 0);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::setIv(const void* iv, std::size_t len)
    {
        static const uint32_t TAU[] =
        { 0x61707865, 0x3120646e, 0x79622d36, 0x6b206574 };

        static const uint32_t SIGMA[] =
        { 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574 };

        _state[4] = _key[0];
        _state[5] = _key[1];
        _state[6] = _key[2];
        _state[7] = _key[3];

        if(_keySize == 4)
        {
            _state[0] = TAU[0];
            _state[1] = TAU[1];
            _state[2] = TAU[2];
            _state[3] = TAU[3];

            _state[8] = _key[0];
            _state[9] = _key[1];
            _state[10] = _key[2];
            _state[11] = _key[3];
        }
        else
        {
            _state[0] = SIGMA[0];
            _state[1] = SIGMA[1];
            _state[2] = SIGMA[2];
            _state[3] = SIGMA[3];

            _state[8] = _key[4];
            _state[9] = _key[5];
            _state[10] = _key[6];
            _state[11] = _key[7];
        }

        _state[12] = 0;
        _state[13] = 0;
        _state[14] = 0;
        _state[15] = 0;

        _position = 0;


        /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
        const uint32_t* iv4 = static_cast<const uint32_t*>(iv);
        if(len == 0)
        {
            // Treat zero length IV same as an all-zero IV
            _state[14] = 0;
            _state[15] = 0;
        }
        else if(len == 8)
        {
            _state[14] = dci::utils::endian::n2l(iv4[0]);
            _state[15] = dci::utils::endian::n2l(iv4[1]);
        }
        else if(len == 12)
        {
            _state[13] = dci::utils::endian::n2l(iv4[0]);
            _state[14] = dci::utils::endian::n2l(iv4[1]);
            _state[15] = dci::utils::endian::n2l(iv4[2]);
        }
        else if(len == 24)
        {
            _state[12] = dci::utils::endian::n2l(iv4[0]);
            _state[13] = dci::utils::endian::n2l(iv4[1]);
            _state[14] = dci::utils::endian::n2l(iv4[2]);
            _state[15] = dci::utils::endian::n2l(iv4[3]);

            std::array<uint32_t, 8> hc {};
            hchacha(hc.data(), _state.data(), _rounds);

            _state[ 4] = hc[0];
            _state[ 5] = hc[1];
            _state[ 6] = hc[2];
            _state[ 7] = hc[3];
            _state[ 8] = hc[4];
            _state[ 9] = hc[5];
            _state[10] = hc[6];
            _state[11] = hc[7];
            _state[12] = 0;
            _state[13] = 0;
            _state[14] = dci::utils::endian::n2l(iv4[4]);
            _state[15] = dci::utils::endian::n2l(iv4[5]);
        }
        else
        {
            dbgWarn("bad iv len provided");
        }

        chacha_x8(_buffer.data(), _state.data(), _rounds);
        _position = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::cipher(const void* in, void* out, std::size_t len)
    {
        uint8_t* out1 = static_cast<uint8_t*>(out);

        if(in)
        {
            const uint8_t* in1 = static_cast<const uint8_t*>(in);
            while(len >= _buffer.size() - _position)
            {
                const size_t available = _buffer.size() - _position;

                for(size_t i(0); i<available; ++i)
                {
                    out1[i] = in1[i] ^ _buffer[_position+i];
                }

                chacha_x8(_buffer.data(), _state.data(), _rounds);

                len -= available;
                in1 += available;
                out1 += available;
                _position = 0;
            }

            for(size_t i(0); i<len; ++i)
            {
                out1[i] = in1[i] ^ _buffer[_position+i];
            }
        }
        else
        {
            while(len >= _buffer.size() - _position)
            {
                const size_t available = _buffer.size() - _position;
                memcpy(out1, _buffer.data()+_position, available);

                chacha_x8(_buffer.data(), _state.data(), _rounds);

                len -= available;
                out1 += available;
                _position = 0;
            }

            memcpy(out1, _buffer.data()+_position, len);
        }

        _position += len;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::seek(std::uint64_t offset)
    {
        // Find the block offset
        const uint64_t counter = offset / 64;

        union
        {
            uint32_t by4[2];
            uint64_t by8[1];
        } out;
        out.by8[0] = dci::utils::endian::n2l(counter);

        _state[12] = dci::utils::endian::n2l(out.by4[0]);
        _state[13] += dci::utils::endian::n2l(out.by4[1]);

        chacha_x8(_buffer.data(), _state.data(), _rounds);
        _position = offset % 64;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::clear()
    {
        _key = std::array<uint32_t, 8>{};
        _keySize = 0;
        _state = std::array<uint32_t, 16>{};
        _buffer = std::array<uint8_t, 8*64>{};
        _position = 0;
    }
}
