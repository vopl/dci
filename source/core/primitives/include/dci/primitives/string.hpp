/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <string>
#include <type_traits>

namespace dci::primitives
{
    //using String    = std::string;
    struct String : std::string
    {
        using Base = std::string;

        using Base::Base;
        String(const Base& b) : Base{b} {}
        String(Base&& b) : Base{std::move(b)} {}

        using Base::operator=;

        String() = default;
        String(const String&) = default;
        String(String&&) = default;

        String& operator=(const String&) = default;
        String& operator=(String&&) = default;

        template <class S>
        requires (std::is_assignable_v<Base&, S&&>)
        String& operator=(S&& s) noexcept(std::is_nothrow_assignable_v<Base&, S&&>)
        {
            Base::operator=(std::forward<S>(s));
            return *this;
        }

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        friend bool operator==(const String& lhs, const String& rhs);
        friend traits_type::comparison_category operator<=>(const String& lhs, const String& rhs);

        template <class Lhs, class Rhs> requires(std::is_same_v<Lhs, String> && !std::is_same_v<Rhs, String>) friend bool operator==(const Lhs& lhs, const Rhs& rhs);
        template <class Lhs, class Rhs> requires(std::is_same_v<Lhs, String> && !std::is_same_v<Rhs, String>) friend traits_type::comparison_category operator<=>(const Lhs& lhs, const Rhs& rhs);

        template <class Lhs, class Rhs> requires(!std::is_same_v<Lhs, String> && std::is_same_v<Rhs, String>) friend bool operator==(const Lhs& lhs, const Rhs& rhs);
        template <class Lhs, class Rhs> requires(!std::is_same_v<Lhs, String> && std::is_same_v<Rhs, String>) friend traits_type::comparison_category operator<=>(const Lhs& lhs, const Rhs& rhs);
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline bool operator==(const String& lhs, const String& rhs)
    {
        return lhs.std() == rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline String::traits_type::comparison_category operator<=>(const String& lhs, const String& rhs)
    {
        return lhs.std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Lhs, class Rhs> requires(std::is_same_v<Lhs, String> && !std::is_same_v<Rhs, String>)
    bool operator==(const Lhs& lhs, const Rhs& rhs)
    {
        return lhs.std() == rhs;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Lhs, class Rhs> requires(std::is_same_v<Lhs, String> && !std::is_same_v<Rhs, String>)
    String::traits_type::comparison_category operator<=>(const Lhs& lhs, const Rhs& rhs)
    {
        return lhs.std() <=> rhs;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Lhs, class Rhs> requires(!std::is_same_v<Lhs, String> && std::is_same_v<Rhs, String>)
    bool operator==(const Lhs& lhs, const Rhs& rhs)
    {
        return lhs == rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Lhs, class Rhs> requires(!std::is_same_v<Lhs, String> && std::is_same_v<Rhs, String>)
    String::traits_type::comparison_category operator<=>(const Lhs& lhs, const Rhs& rhs)
    {
        return lhs <=> rhs.std();
    }
}
