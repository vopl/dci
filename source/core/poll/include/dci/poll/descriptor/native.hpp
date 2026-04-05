/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
