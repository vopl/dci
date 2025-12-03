// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/crypto/streamCipher.hpp>
#include "impl/streamCipher.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::StreamCipher(himpl::FakeConstructionArg fc)
        : himpl::FaceLayout<StreamCipher, impl::StreamCipher>(fc)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    StreamCipher::~StreamCipher()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::setKey(const void* key, std::size_t len)
    {
        return impl().setKey(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::setIv(const void* iv, std::size_t len)
    {
        return impl().setIv(iv, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::cipher(const void* in, void* out, std::size_t len)
    {
        return impl().cipher(in, out, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::seek(std::uint64_t offset)
    {
        return impl().seek(offset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::clear()
    {
        return impl().clear();
    }
}
