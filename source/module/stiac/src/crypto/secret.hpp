// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/bytes.hpp>
#include <array>
#include <type_traits>

namespace dci::module::stiac::crypto
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size_>
    struct Secret
        : std::array<std::uint8_t, size_>
    {
        Secret();
        ~Secret();

        using std::array<std::uint8_t, size_>::operator=;
        Secret& operator=(const Secret&) = default;

        static std::uint32_t size();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void cleanMemoryUnder(void* memory, std::uint32_t size);
    void cleanMemoryUnder(dci::Bytes& v);
    template <class T> void cleanMemoryUnder(T& v) requires std::is_trivially_copyable_v<T>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size_>
    Secret<size_>::Secret()
        : std::array<std::uint8_t, size_>{}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size_>
    Secret<size_>::~Secret()
    {
        cleanMemoryUnder(this->data(), this->size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size_>
    std::uint32_t Secret<size_>::size()
    {
        return size_;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void cleanMemoryUnder(void* memory, std::uint32_t size)
    {
        volatile std::uint8_t* d = const_cast<volatile std::uint8_t *>(static_cast<std::uint8_t *>(memory));
        for(std::size_t i(0); i<size; ++i)
        {
            d[i] = 0;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    void cleanMemoryUnder(T& v) requires std::is_trivially_copyable_v<T>
    {
        cleanMemoryUnder(&v, sizeof(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline void cleanMemoryUnder(dci::Bytes& v)
    {
        dci::bytes::Alter a(v.begin());
        while(!a.atEnd())
        {
            cleanMemoryUnder(a.continuousData4Write(), a.continuousDataSize());
            a.advanceChunks(1);
        }

    }

}
