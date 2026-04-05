/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <variant>
#include <dci/primitives.hpp>
#include "../smallIntegral.hpp"

namespace dci::stiac::serialization
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    void save(auto& ar, const std::variant<Ts...>& v)
    {
        if(std::variant_npos == v.index())
        {
            ar << smallIntegral(uint32{0});
            return;
        }

        ar << smallIntegral(static_cast<uint32>(v.index())+1);

        std::visit([&](const auto& v)
        {
            ar << v;
        }, v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    void save(auto& ar, std::variant<Ts...>&& v)
    {
        if(std::variant_npos == v.index())
        {
            ar << smallIntegral(uint32{0});
            return;
        }

        ar << smallIntegral(static_cast<uint32>(v.index())+1);

        std::visit([&](auto&& v)
        {
            ar << std::move(v);
        }, std::move(v));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        struct BadVariantMaker
        {
            struct Exception {};

            template <class Any>
            operator Any() const
            {
                throw Exception();
            }
        };

        template <std::size_t currentIndex, class... T>
        void loadVariantMember(auto& ar, std::size_t wantedIndex, std::variant<T...>& v)
        {
            if(wantedIndex == currentIndex)
            {
                ar >> v.template emplace<currentIndex>();
                return;
            }

            if constexpr(currentIndex+1 < sizeof...(T))
            {
                loadVariantMember<currentIndex+1, T...>(ar, wantedIndex, v);
                return;
            }
            else
            {
                throw "malformed input";
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Ts>
    void load(auto& ar, std::variant<Ts...>& v)
    {
        uint32 index;
        ar >> smallIntegral(index);

        if(!index)
        {
            try
            {
                v.template emplace<0>(details::BadVariantMaker());
            }
            catch(details::BadVariantMaker::Exception)
            {
                //ignore
            }
            return;
        }

        details::loadVariantMember<0, Ts...>(ar, index-1, v);
    }
}
