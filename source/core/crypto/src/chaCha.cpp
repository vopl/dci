// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/crypto/chaCha.hpp>
#include "impl/chaCha.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::ChaCha(std::size_t rounds)
        : himpl::FaceLayout<ChaCha, impl::ChaCha, StreamCipher>(rounds)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::ChaCha(const ChaCha& from)
        : himpl::FaceLayout<ChaCha, impl::ChaCha, StreamCipher>(from.impl())
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::ChaCha(ChaCha&& from)
        : himpl::FaceLayout<ChaCha, impl::ChaCha, StreamCipher>(std::move(from))
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha& ChaCha::operator=(const ChaCha& from)
    {
        impl() = from.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha& ChaCha::operator=(ChaCha&& from)
    {
        impl() = std::move(from.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ChaCha::~ChaCha()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::setKey(const void* key, std::size_t len)
    {
        return impl().setKey(key, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::setIv(const void* iv, std::size_t len)
    {
        return impl().setIv(iv, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::cipher(const void* in, void* out, std::size_t len)
    {
        return impl().cipher(in, out, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::seek(std::uint64_t offset)
    {
        return impl().seek(offset);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void ChaCha::clear()
    {
        return impl().clear();
    }
}
