// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "streamCipher.hpp"
#include <cstdlib>
#include <dci/utils/dbg.hpp>

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher(const StreamCipher&)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher(StreamCipher&&)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::~StreamCipher()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher& StreamCipher::operator=(const StreamCipher&)
    {
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher& StreamCipher::operator=(StreamCipher&&)
    {
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::setKey(const void* key, std::size_t len)
    {
        (void)key;
        (void)len;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::setIv(const void* iv, std::size_t len)
    {
        (void)iv;
        (void)len;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::cipher(const void* in, void* out, std::size_t len)
    {
        (void)in;
        (void)out;
        (void)len;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::seek(std::uint64_t offset)
    {
        (void)offset;
        dbgWarn("must be overrided!");
        abort();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::clear()
    {
        dbgWarn("must be overrided!");
        abort();
    }
}
