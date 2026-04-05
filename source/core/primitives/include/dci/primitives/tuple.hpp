/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <tuple>

namespace dci::primitives
{
    template <class... T>
    struct Tuple : std::tuple<T...>
    {
        using Base = std::tuple<T...>;

        using Base::Base;
        using Base::operator=;

        Tuple(const Base& b) : Base{b} {}
        Tuple(Base&& b) : Base{std::move(b)} {}

        Tuple() : Base{} {};
        Tuple(const Tuple&) = default;
        Tuple(Tuple&&) = default;

        Tuple& operator=(const Tuple&) = default;
        Tuple& operator=(Tuple&&) = default;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        static constexpr std::size_t _size = sizeof... (T);
        template <std::size_t I> using Element = std::tuple_element_t<I, Base>;

        template <std::size_t I> decltype(auto) get() &       {return std::get<I>(std());}
        template <std::size_t I> decltype(auto) get() const & {return std::get<I>(std());}
        template <std::size_t I> decltype(auto) get() &&      {return std::get<I>(std::move(std()));}
        template <std::size_t I> decltype(auto) get() const &&{return std::get<I>(std::move(std()));}

        decltype(auto) apply(auto&& f)       & {return std::apply(std::forward<decltype(f)>(f), std());}
        decltype(auto) apply(auto&& f) const & {return std::apply(std::forward<decltype(f)>(f), std());}
        decltype(auto) apply(auto&& f)       &&{return std::apply(std::forward<decltype(f)>(f), std::move(std()));}
        decltype(auto) apply(auto&& f) const &&{return std::apply(std::forward<decltype(f)>(f), std::move(std()));}

        auto operator<=>(const Tuple& rhs) const;
        bool operator==(const Tuple& rhs) const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... T>
    auto Tuple<T...>::operator<=>(const Tuple& rhs) const
    {
        return std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... T>
    bool Tuple<T...>::operator==(const Tuple& rhs) const
    {
        return std() == rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<typename... T> Tuple(T...) -> Tuple<T...>;
    template<typename T1, typename T2> Tuple(std::pair<T1, T2>) -> Tuple<T1, T2>;
    template<typename... T> Tuple(std::tuple<T...>) -> Tuple<T...>;
}

namespace std
{
    template <class... T> struct tuple_size<dci::primitives::Tuple<T...>> : std::integral_constant<std::size_t, dci::primitives::Tuple<T...>::_size> {};
    template <size_t I, class... T> struct tuple_element<I, dci::primitives::Tuple<T...>> { using type = typename dci::primitives::Tuple<T...>::template Element<I>; };

    template <std::size_t I, class... T> decltype(auto) get(dci::primitives::Tuple<T...>& t       ) {return std::forward<decltype(t)>(t).template get<I>();}
    template <std::size_t I, class... T> decltype(auto) get(dci::primitives::Tuple<T...> const& t ) {return std::forward<decltype(t)>(t).template get<I>();}
    template <std::size_t I, class... T> decltype(auto) get(dci::primitives::Tuple<T...>&& t      ) {return std::forward<decltype(t)>(t).template get<I>();}
    template <std::size_t I, class... T> decltype(auto) get(dci::primitives::Tuple<T...> const&& t) {return std::forward<decltype(t)>(t).template get<I>();}
}
