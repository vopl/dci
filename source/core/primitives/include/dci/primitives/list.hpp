// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <deque>

namespace dci::primitives
{
    template <class T>
    struct List : std::deque<T>
    {
        using Base = std::deque<T>;

        using Base::Base;
        using Base::operator=;

        List() = default;
        List(const List&) = default;
        List(List&&) = default;

        List& operator=(const List&) = default;
        List& operator=(List&&) = default;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        auto operator<=>(const List& rhs) const;
        bool operator==(const List& rhs) const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    auto List<T>::operator<=>(const List& rhs) const
    {
        return std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    bool List<T>::operator==(const List& rhs) const
    {
        return std() == rhs.std();
    }
}
