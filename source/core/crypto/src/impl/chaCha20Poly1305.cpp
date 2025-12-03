// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "chaCha20Poly1305.hpp"
#include <dci/utils/endian.hpp>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::ChaCha20Poly1305()
        : _chaCha{20}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::ChaCha20Poly1305(const ChaCha20Poly1305& from)
        : _chaCha{from._chaCha}
        , _poly1305{from._poly1305}
        , _ad{from._ad}
        , _nonceLen{from._nonceLen}
        , _ctextLen{from._ctextLen}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::ChaCha20Poly1305(ChaCha20Poly1305&& from)
        : _chaCha{std::move(from._chaCha)}
        , _poly1305{std::move(from._poly1305)}
        , _ad{std::move(from._ad)}
        , _nonceLen{std::move(from._nonceLen)}
        , _ctextLen{std::move(from._ctextLen)}
    {
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305::~ChaCha20Poly1305()
    {
        _chaCha.clear();
        _poly1305.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305& ChaCha20Poly1305::operator=(const ChaCha20Poly1305& from)
    {
        _chaCha = from._chaCha;
        _poly1305 = from._poly1305;
        _ad = from._ad;
        _nonceLen = from._nonceLen;
        _ctextLen = from._ctextLen;

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha20Poly1305& ChaCha20Poly1305::operator=(ChaCha20Poly1305&& from)
    {
        _chaCha = std::move(from._chaCha);
        _poly1305 = std::move(from._poly1305);
        _ad = std::move(from._ad);
        _nonceLen = std::move(from._nonceLen);
        _ctextLen = std::move(from._ctextLen);

        from.clear();

        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::setKey(const void* key, std::size_t len)
    {
        _chaCha.setKey(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::setAd(const void* ad, std::size_t len)
    {
        const std::uint8_t* ad1 = static_cast<const std::uint8_t*>(ad);
        _ad.assign(ad1, ad1 + len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::start(const void* nonce, std::size_t len)
    {
        _ctextLen = 0;
        _nonceLen = len;

        _chaCha.setIv(nonce, len);

        std::array<uint8_t, 64> firstBlock {};
        _chaCha.cipher(firstBlock.data(), firstBlock.data(), firstBlock.size());

        _poly1305.setKey(firstBlock.data(), 32);
        // Remainder of first block is discarded

        _poly1305.add(_ad.data(), _ad.size());

        if(cfrgVersion())
        {
            if(_ad.size() % 16)
            {
                const uint8_t zeros[16] = { 0 };
               _poly1305.add(zeros, 16 - _ad.size() % 16);
            }
        }
        else
        {
            updateLen(_ad.size());
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::encipher(const void* in, void* out, std::size_t len)
    {
        _chaCha.cipher(in, out, len);
        _poly1305.add(out, len); // poly1305 of ciphertext
        _ctextLen += len;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::encipherFinish(void* macOut)
    {
        if(cfrgVersion())
        {
            if(_ctextLen % 16)
            {
                const uint8_t zeros[16] = { 0 };
                _poly1305.add(zeros, 16 - _ctextLen % 16);
            }
            updateLen(_ad.size());
        }
        updateLen(_ctextLen);

        _poly1305.finish(macOut);
        _ctextLen = 0;
        _nonceLen = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::decipher(const void* in, void* out, std::size_t len)
    {
        _poly1305.add(in, len); // poly1305 of ciphertext
        _chaCha.cipher(in, out, len);
        _ctextLen += len;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool ChaCha20Poly1305::decipherFinish(const void* macIn)
    {
        if(cfrgVersion())
        {
            if(_ctextLen % 16)
            {
                const uint8_t zeros[16] = { 0 };
                _poly1305.add(zeros, 16 - _ctextLen % 16);
            }
            updateLen(_ad.size());
        }
        updateLen(_ctextLen);

        _ctextLen = 0;
        _nonceLen = 0;

        std::array<uint8_t, 16> mac;
        _poly1305.finish(mac.data());

        const uint8_t* macIn1 = static_cast<const uint8_t* >(macIn);

        uint8_t diff = 0;

        for(size_t i(0); i<mac.size(); ++i)
        {
            diff |= macIn1[i] ^ mac[i];
        }

        return 0 == diff;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::clear()
    {
        _chaCha.clear();
        _poly1305.clear();
        _ad.clear();
        _nonceLen = 0;
        _ctextLen = 0;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool ChaCha20Poly1305::cfrgVersion() const
    {
        return _nonceLen==12 || _nonceLen==24;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha20Poly1305::updateLen(std::size_t len)
    {
        union
        {
            std::uint8_t  by1[8];
            std::uint64_t by8[1];
        } l;
        l.by8[0] = dci::utils::endian::n2l(static_cast<uint64_t>(len));
        _poly1305.add(l.by1, 8);
    }

}
