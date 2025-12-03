// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include <type_traits>

namespace dci::crypto::impl
{
    class StreamCipher
    {
    public:
        StreamCipher();
        StreamCipher(const StreamCipher&);
        StreamCipher(StreamCipher&&);
        virtual ~StreamCipher();
        static void tryDestruction(auto*);

        StreamCipher& operator=(const StreamCipher&);
        StreamCipher& operator=(StreamCipher&&);

    public:
        virtual void setKey(const void* key, std::size_t len);
        virtual void setIv(const void* iv, std::size_t len);
        virtual void cipher(const void* in, void* out, std::size_t len);
        virtual void seek(std::uint64_t offset);
        virtual void clear();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void StreamCipher::tryDestruction(auto* o)
    {
        using C = std::decay_t<decltype(*o)>;
        if constexpr(std::is_same_v<StreamCipher, C>)
        {
            o->~C();
        }
    }
}
