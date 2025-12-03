// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <set>

namespace dci::primitives
{
    template <class T>
    struct Set : std::set<T>
    {
        using Base = std::set<T>;

        using Base::Base;
        using Base::operator=;

        Set() = default;
        Set(const Set&) = default;
        Set(Set&&) = default;

        Set& operator=(const Set&) = default;
        Set& operator=(Set&&) = default;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        auto operator<=>(const Set& rhs) const;
        bool operator==(const Set& rhs) const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    auto Set<T>::operator<=>(const Set& rhs) const
    {
        return std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool Set<T>::operator==(const Set& rhs) const
    {
        return std() == rhs.std();
    }
}
