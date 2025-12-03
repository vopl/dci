// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <map>

namespace dci::primitives
{
    template <class K, class V>
    struct Map : std::map<K, V>
    {
        using Base = std::map<K, V>;

        using Base::Base;
        using Base::operator=;

        Map() = default;
        Map(const Map&) = default;
        Map(Map&&) = default;

        Map& operator=(const Map&) = default;
        Map& operator=(Map&&) = default;

              Base&  std()       & {return *this;}
        const Base&  std() const & {return *this;}
              Base&& std()       &&{return std::move(*this);}
        const Base&& std() const &&{return std::move(*this);}

        auto operator<=>(const Map& rhs) const;
        bool operator==(const Map& rhs) const;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class K, class V>
    auto Map<K, V>::operator<=>(const Map& rhs) const
    {
        return std() <=> rhs.std();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class K, class V>
    bool Map<K, V>::operator==(const Map& rhs) const
    {
        return std() == rhs.std();
    }
}
