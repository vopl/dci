// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include <concepts>
#include <utility>
#include <compare>

namespace dci::poll::descriptor
{
    struct Native
    {
#ifdef _WIN32
        using Value = std::uintptr_t;
        static constexpr Value _bad = ~Value{};
#else
        using Value = int;
        static constexpr Value _bad = -1;
#endif

        Value _value{_bad};

        Native() = default;

        template <class T>
        Native(T&& value) requires(std::constructible_from<Value, T&&>)
            : _value(std::forward<T>(value))
        {
        }

        operator Value() const
        {
            return _value;
        };

        template <class T>
        Native& operator=(T&& value) requires(std::assignable_from<Value, T&&>)
        {
            _value = std::forward<T>(value);
            return *this;
        }

        auto operator<=>(const Native&) const = default;
    };
}
