// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/crypto/sha2_256.hpp>
#include "impl/sha2_256.hpp"

namespace dci::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Sha2_256::alloc(std::size_t digestSize)
    {
        return HashPtr
        {
            new Sha2_256{digestSize},
            [](Hash* p){delete static_cast<Sha2_256*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::Sha2_256(std::size_t digestSize)
        : himpl::FaceLayout<Sha2_256, impl::Sha2_256, Hash>{digestSize}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::Sha2_256(const Sha2_256& from)
        : himpl::FaceLayout<Sha2_256, impl::Sha2_256, Hash>{from.impl()}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::Sha2_256(Sha2_256&& from)
        : himpl::FaceLayout<Sha2_256, impl::Sha2_256, Hash>{std::move(from.impl())}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256::~Sha2_256()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256& Sha2_256::operator=(const Sha2_256& from)
    {
        impl() = from.impl();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sha2_256& Sha2_256::operator=(Sha2_256&& from)
    {
        impl() = std::move(from.impl());
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Sha2_256::clone()
    {
        return impl().clone();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Sha2_256::blockSize()
    {
        return impl().blockSize();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::add(const void* data, std::size_t len)
    {
        return impl().add(data, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::barrier()
    {
        return impl().barrier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::finish(void* digest)
    {
        return impl().finish(digest);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sha2_256::finish(void* digest, std::size_t customDigestSize)
    {
        return impl().finish(digest, customDigestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void sha2_256(const void* data, std::size_t len, void* digest, std::size_t digestSize)
    {
        impl::Sha2_256 impl{digestSize};
        impl.add(data, len);
        impl.finish(digest);
    }
}
