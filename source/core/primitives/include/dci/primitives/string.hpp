// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
