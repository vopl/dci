// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/utils/b2h.hpp>

namespace dci::utils
{
    namespace
    {
        char dgt(uint8_t h)
        {
            if(h < 10)
            {
                return static_cast<char>('0'+h);
            }
            return static_cast<char>('a'+h-10);
        }

        void b2h_little(const std::uint8_t* b, std::size_t bsize, char* h)
        {
            for(std::size_t i(0); i<bsize; ++i)
            {
                h[i*2+0] = dgt(b[i]&0xf);
                h[i*2+1] = dgt(b[i]>>4);
            }
        }

        void b2h_middle(const std::uint8_t* b, std::size_t bsize, char* h)
        {
            for(std::size_t i(0); i<bsize; ++i)
            {
                h[i*2+0] = dgt(b[i]>>4);
                h[i*2+1] = dgt(b[i]&0xf);
            }
        }

        void b2h_big(const std::uint8_t* b, std::size_t bsize, char* h)
        {
            for(std::size_t i(0); i<bsize; ++i)
            {
                h[i*2+0] = dgt(b[bsize-1-i]>>4);
                h[i*2+1] = dgt(b[bsize-1-i]&0xf);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void b2h(const void* b, std::size_t bsize, void* h, HexEndian he)
    {
        const std::uint8_t* b_ = static_cast<const std::uint8_t *>(b);
        char* h_ = static_cast<char *>(h);

        switch(he)
        {
        case HexEndian::little:
            return b2h_little(b_, bsize, h_);
        case HexEndian::middle:
            return b2h_middle(b_, bsize, h_);
        case HexEndian::big:
            return b2h_big(b_, bsize, h_);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::string b2h(const void* b, std::size_t bsize, HexEndian he)
    {
        std::string h;
        h.resize(bsize*2);
        b2h(b, bsize, h.data(), he);
        return h;
    }
}
