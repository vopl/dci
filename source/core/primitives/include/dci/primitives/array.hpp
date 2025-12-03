// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
