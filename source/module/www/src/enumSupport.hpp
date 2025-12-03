// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::enumSupport
{
    template <class E> std::optional<std::string_view> toString(E e);
    template <class E> std::optional<E> toEnum(std::string_view s);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <int size, bool lowerCase = false>
    constexpr auto asKey(const char* str)
    {
        using Uint = dci::utils::integer::uintCover<size * CHAR_BIT>;
        char arr[sizeof(Uint)]{};
        for(std::size_t i{}; i<size; ++i)
        {
            char c = str[i];
            if constexpr(lowerCase)
            {
                if('A' <= c && c <= 'Z')
                    c = c - 'A' + 'a';
            }
            arr[i] = c;
        }

        return std::bit_cast<Uint>(arr);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <int size, bool lowerCase>
    constexpr auto asKey(const char (&str)[size])
    {
        return asKey<size-1, lowerCase>(static_cast<const char*>(str));
    }
}
