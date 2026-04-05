/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
