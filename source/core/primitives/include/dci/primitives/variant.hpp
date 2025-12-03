// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "integral.hpp"
#include <variant>
#include <cstdint>

namespace dci::primitives
{
    template <class... Ts>
    class Variant
        : public std::variant<Ts...>
    {
    public:
        using std::variant<Ts...>::variant;

              std::variant<Ts...>&  std()       &;
        const std::variant<Ts...>&  std() const &;
              std::variant<Ts...>&& std()       &&;
        const std::variant<Ts...>&& std() const &&;

        template <class T> static constexpr bool canHold();

        template <class T> constexpr bool holds() const;
        constexpr uint32 index() const;
        static constexpr uint32 size();

        decltype(auto) visit(auto&& f)       &;
        decltype(auto) visit(auto&& f) const &;
        decltype(auto) visit(auto&& f)       &&;
        decltype(auto) visit(auto&& f) const &&;

        template <class T>       T&  get()       &;
        template <class T> const T&  get() const &;
        template <class T>       T&& get()       &&;
        template <class T> const T&& get() const &&;

        template <class T> const T&  getOr(const T& def = T{}) const &;

        template <class T> T&  sget() &;
        template <class T> T&& sget() &&;

        auto operator<=>(const Variant& rhs) const;
        bool operator==(const Variant& rhs) const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    std::variant<Ts...>& Variant<Ts...>::std() &
    {
        return static_cast<std::variant<Ts...>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    const std::variant<Ts...>& Variant<Ts...>::std() const &
    {
        return static_cast<const std::variant<Ts...>&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    std::variant<Ts...>&& Variant<Ts...>::std() &&
    {
        return static_cast<std::variant<Ts...>&&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    const std::variant<Ts...>&& Variant<Ts...>::std() const &&
    {
        return static_cast<const std::variant<Ts...>&&>(*this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T> constexpr bool Variant<Ts...>::canHold()
    {
        return (std::is_same_v<T, Ts> || ...);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T> constexpr bool Variant<Ts...>::holds() const
    {
        return std::holds_alternative<T>(std());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    constexpr uint32 Variant<Ts...>::index() const
    {
        return static_cast<uint32>(std().index());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    constexpr uint32 Variant<Ts...>::size()
    {
        return std::variant_size_v<std::variant<Ts...>>;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    decltype(auto) Variant<Ts...>::visit(auto&& f) &
    {
        return std::visit(std::forward<decltype(f)>(f), std());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    decltype(auto) Variant<Ts...>::visit(auto&& f) const &
    {
        return std::visit(std::forward<decltype(f)>(f), std());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    decltype(auto) Variant<Ts...>::visit(auto&& f) &&
    {
        return std::visit(std::forward<decltype(f)>(f), std::move(std()));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    decltype(auto) Variant<Ts...>::visit(auto&& f) const &&
    {
        return std::visit(std::forward<decltype(f)>(f), std::move(std()));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T>
    T& Variant<Ts...>::get() &
    {
        return std::get<T>(std());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T>
    const T& Variant<Ts...>::get() const &
    {
        return std::get<T>(std());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T>
    T&& Variant<Ts...>::get() &&
    {
        return std::get<T>(std::move(std()));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T>
    const T&& Variant<Ts...>::get() const &&
    {
        return std::get<T>(std::move(std()));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T>
    const T& Variant<Ts...>::getOr(const T& def) const &
    {
        if(!holds<T>())
        {
            return def;
        }

        return get<T>();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T> T&
    Variant<Ts...>::sget() &
    {
        if(!std::holds_alternative<T>(std()))
        {
            std() = T();
        }
        return get<T>();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    template <class T> T&&
    Variant<Ts...>::sget() &&
    {
        if(!std::holds_alternative<T>(std()))
        {
            std() = T();
        }
        return std::move(get<T>());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    auto Variant<Ts...>::operator<=>(const Variant& rhs) const
    {
        return std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    bool Variant<Ts...>::operator==(const Variant& rhs) const
    {
        return std() == rhs.std();
    }
}
