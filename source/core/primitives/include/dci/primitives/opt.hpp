// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <optional>

namespace dci::primitives
{
    template <class T>
    struct Opt : std::optional<T>
    {
        using Base = std::optional<T>;

        using Base::Base;
        using Base::operator=;

        Opt() = default;
        Opt(const Opt&) = default;
        Opt(Opt&&) = default;

        Opt& operator=(const Opt&) = default;
        Opt& operator=(Opt&&) = default;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        friend bool operator==(const Opt& lhs, const Opt& rhs) = default;
        friend auto operator<=>(const Opt& lhs, const Opt& rhs) = default;

        friend bool operator==(const Opt& lhs, std::nullptr_t)
        {
            return bool(lhs) == false;
        }

        friend auto operator<=>(const Opt& lhs, std::nullptr_t)
        {
            return bool(lhs) <=> false;
        }

        friend bool operator==(std::nullptr_t, const Opt& rhs)
        {
            return false == bool(rhs);
        }

        friend auto operator<=>(std::nullptr_t, const Opt& rhs)
        {
            return false <=> bool(rhs);
        }

        friend bool operator==(const Opt& lhs, const T& rhs)
        {
            return bool(lhs) ? *lhs == rhs : false;
        }

        friend auto operator<=>(const Opt& lhs, const T& rhs)
        {
            return bool(lhs) ? *lhs <=> rhs : std::strong_ordering::less;
        }

        friend bool operator==(const T& lhs, const Opt& rhs)
        {
            return bool(rhs) ? lhs == *rhs : false;
        }

        friend auto operator<=>(const T& lhs, const Opt& rhs)
        {
            return bool(rhs) ? lhs <=> *rhs : std::strong_ordering::greater;
        }
    };
}

#if defined(__GNUC__) && defined(_GLIBCXX_OPTIONAL)
namespace std
{
    template<typename _Tp>
      inline constexpr bool __is_optional_v<dci::primitives::Opt<_Tp>> = true;
}
#endif
