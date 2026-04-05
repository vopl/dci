/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <array>
#include <tuple>

namespace dci::primitives
{
    template <class T, std::size_t N>
    struct alignas(void*) Array : std::array<T, N>
    {
        using Base = std::array<T, N>;

        using Base::operator=;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        static constexpr std::size_t _size = N;

        template <std::size_t I> decltype(auto) get() &       {return std::get<I>(std());}
        template <std::size_t I> decltype(auto) get() const & {return std::get<I>(std());}
        template <std::size_t I> decltype(auto) get() &&      {return std::get<I>(std::move(std()));}
        template <std::size_t I> decltype(auto) get() const &&{return std::get<I>(std::move(std()));}

        decltype(auto) apply(auto&& f)       & {return std::apply(std::forward<decltype(f)>(f), std());}
        decltype(auto) apply(auto&& f) const & {return std::apply(std::forward<decltype(f)>(f), std());}
        decltype(auto) apply(auto&& f)       &&{return std::apply(std::forward<decltype(f)>(f), std::move(std()));}
        decltype(auto) apply(auto&& f) const &&{return std::apply(std::forward<decltype(f)>(f), std::move(std()));}

        auto operator<=>(const Array& rhs) const;
        bool operator==(const Array& rhs) const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, std::size_t N>
    auto Array<T, N>::operator<=>(const Array& rhs) const
    {
        return std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, std::size_t N>
    bool Array<T, N>::operator==(const Array& rhs) const
    {
        return std() == rhs.std();
    }
}

namespace std
{
    template <class T, std::size_t N> struct tuple_size<dci::primitives::Array<T, N>> : std::integral_constant<std::size_t, N> {};
    template <size_t I, class T, std::size_t N> struct tuple_element<I, dci::primitives::Array<T, N>> { using type = T; };

    template <std::size_t I, class T, std::size_t N> decltype(auto) get(dci::primitives::Array<T, N>& t       ) {return std::forward<decltype(t)>(t).template get<I>();}
    template <std::size_t I, class T, std::size_t N> decltype(auto) get(dci::primitives::Array<T, N> const& t ) {return std::forward<decltype(t)>(t).template get<I>();}
    template <std::size_t I, class T, std::size_t N> decltype(auto) get(dci::primitives::Array<T, N>&& t      ) {return std::forward<decltype(t)>(t).template get<I>();}
    template <std::size_t I, class T, std::size_t N> decltype(auto) get(dci::primitives::Array<T, N> const&& t) {return std::forward<decltype(t)>(t).template get<I>();}
}
