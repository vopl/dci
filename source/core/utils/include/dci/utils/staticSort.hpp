/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <bit>
#include <array>
#include <functional>
#include <utility>

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size, class V, class Cmp = std::less<V>> constexpr void staticSort(V* array);
    template <std::size_t size, class V, class Cmp               > constexpr void staticSort(V* array, Cmp&& cmp);

    template <std::size_t size, class V, class Cmp = std::less<V>> constexpr void staticSort(V (&array)[size]);
    template <std::size_t size, class V, class Cmp               > constexpr void staticSort(V (&array)[size], Cmp&& cmp);

    template <std::size_t size, class V, class Cmp = std::less<V>> constexpr void staticSort(std::array<V, size>& array);
    template <std::size_t size, class V, class Cmp               > constexpr void staticSort(std::array<V, size>& array, Cmp&& cmp);
}

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size, class V, class Cmp> class StaticSort
    {
        V* _array;
        Cmp _cmp;

    public:
        constexpr StaticSort(V* array)
            : _array{array}
            , _cmp{}
        {}

        constexpr StaticSort(V* array, Cmp&& cmp)
            : _array{array}
            , _cmp{std::move(cmp)}
        {}

        constexpr StaticSort(V* array, const Cmp& cmp)
            : _array{array}
            , _cmp{cmp}
        {}

    public:
        template <std::size_t offset=0, std::size_t stride=1, std::size_t width=size>
        constexpr void exec()
        {
            if constexpr(0 == width || 1 == width)
                ;
            else if constexpr(2 == width)
                swap<offset, offset + stride>();
            else
            {
                constexpr std::size_t width1 = std::size_t{1} << (std::bit_width((2 * width) / 3) - 1);
                constexpr std::size_t width2 = width - width1;
                exec<offset                  , stride, width1>();
                exec<offset + width1 * stride, stride, width2>();
                merge<offset, stride, width1, width2>();
            }
        }

    private:
        template <std::size_t offset, std::size_t stride, std::size_t width1, std::size_t width2>
        constexpr void merge()
        {
            if constexpr(0 == width1 || 0 == width2)
                ;
            else if constexpr(1 == width1 && 1 == width2)
                swap<offset, offset + stride>();
            else if constexpr(1 == width1 && 2 == width2)
            {
                swap<offset, offset + 2*stride>();
                swap<offset, offset +   stride>();
            }
            else if constexpr(2 == width1 && 1 == width2)
            {
                swap<offset         , offset + 2*stride>();
                swap<offset + stride, offset + 2*stride>();
            }
            else
            {
                static_assert(1 == std::popcount(width1), "width1 must be a power of 2");
                static_assert(width1 <= 2*width2 && width2 <= 2*width1, "width1 and width2 must be balanced.");

                merge<offset,          2*stride, width1/2, (width2 + 1)/2>();
                merge<offset + stride, 2*stride, width1/2,  width2     /2>();

                finalize<offset + stride, stride, (width1 + width2 - 1)/2>();
            }
        }

    private:
        template <std::size_t offset, std::size_t stride, std::size_t width>
        constexpr void finalize()
        {
            if constexpr(0 == width)
                ;
            else
            {
                swap<offset, offset + stride>();
                finalize<offset + 2*stride, stride, width - 1>();
            }
        }

    private:
        template <std::size_t offset1, std::size_t offset2>
        constexpr void swap()
        {
            V& a = _array[offset1];
            V& b = _array[offset2];

            if(_cmp(b, a))
                std::swap(a, b);
        }
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size, class V, class Cmp> constexpr void staticSort(V* array)
    {
        StaticSort<size, V, Cmp>{array}.exec();
    }

    template <std::size_t size, class V, class Cmp> constexpr void staticSort(V* array, Cmp&& cmp)
    {
        StaticSort<size, V, std::decay_t<Cmp>>{array, std::forward<Cmp>(cmp)}.exec();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size, class V, class Cmp> constexpr void staticSort(V (&array)[size])
    {
        StaticSort<size, V, Cmp>{static_cast<V*>(array)}.exec();
    }

    template <std::size_t size, class V, class Cmp> constexpr void staticSort(V (&array)[size], Cmp&& cmp)
    {
        StaticSort<size, V, std::decay_t<Cmp>>{static_cast<V*>(array), std::forward<Cmp>(cmp)}.exec();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <std::size_t size, class V, class Cmp> constexpr void staticSort(std::array<V, size>& array)
    {
        StaticSort<size, V, Cmp>{array.data()}.exec();
    }

    template <std::size_t size, class V, class Cmp> constexpr void staticSort(std::array<V, size>& array, Cmp&& cmp)
    {
        StaticSort<size, V, std::decay_t<Cmp>>{array.data(), std::forward<Cmp>(cmp)}.exec();
    }
}
