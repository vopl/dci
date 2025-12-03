// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once
#include <dci/crypto/hashPtr.hpp>
#include <cstring>
#include <type_traits>

namespace dci::crypto::impl
{
    class Hash
    {
    public:
        Hash(std::size_t digestSize);
        Hash(const Hash&);
        Hash(Hash&&);
        virtual ~Hash();
        static void tryDestruction(auto*);

        Hash& operator=(const Hash&);
        Hash& operator=(Hash&&);

        virtual HashPtr clone() = 0;

    public:
        virtual std::size_t blockSize() = 0;
        virtual std::size_t digestSize();
        virtual void add(const void* data, std::size_t len) = 0;
        virtual void barrier() = 0;
        virtual void finish(void* digest) = 0;
        virtual void finish(void* digest, std::size_t customDigestSize) = 0;
        virtual void clear() = 0;

    protected:
        std::size_t _digestSize;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Hash::tryDestruction(auto*o)
    {
        using C = std::decay_t<decltype(*o)>;
        if constexpr(std::is_same_v<Hash, C>)
        {
            o->~C();
        }
    }
}
