/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../fakeConstructionArg.hpp"
#include "../implMetaInfo.hpp"
#include "metaInfoFetcher.hpp"
#include "space.hpp"

namespace dci::himpl::details
{

    //Размер префикса - смещение первого базового импла от текущего импла
    template <class... T>
    class SimpleAreaPrefix;

    template <class TImpl, class TFirstBase, class... TOtherBases>
    class
        alignas(ImplMetaInfo<TImpl>::_align)
        SimpleAreaPrefix<TImpl, TFirstBase, TOtherBases...>
        : public Space<
                    std::integral_constant<std::size_t, ImplMetaInfo<TImpl>::template baseOffset<typename TFirstBase::Impl>()>,
                    SimpleAreaPrefix<TImpl, TFirstBase, TOtherBases...>
                 >
    {
    };

    template <class TImpl>
    class
        alignas(ImplMetaInfo<TImpl>::_align)
        SimpleAreaPrefix<TImpl>
    {
    };


    //Размер суффикса - от размера текущего импла отнять (размер префикса + размер базовых классов)
    template <class TImpl, class... TBases>
    struct SimpleFaceLayoutPrefixPlusBases
            : private SimpleAreaPrefix<TImpl, TBases...>
            , public TBases...
    {
    };

    template <class TImpl, class... TBases>
    struct SimpleFaceLayoutProberForSuffix
    {
        using Prober = SimpleFaceLayoutPrefixPlusBases<TImpl, TBases...>;
        static constexpr std::size_t _value = MetaInfoFetcher<Prober>::_size;
    };


    template <class TImpl, class... TBases>
    class SimpleAreaSuffix
        : public Space<
            std::integral_constant<std::size_t,  ImplMetaInfo<TImpl>::_size - SimpleFaceLayoutProberForSuffix<TImpl, TBases...>::_value>,
            SimpleAreaSuffix<TImpl, TBases...>>
    {
    };

    //нет полиморфности
    template <class TImpl, class... TBases>
    class SimpleFaceLayout
        : private SimpleAreaPrefix<TImpl, TBases...>
        , public TBases...
        , private SimpleAreaSuffix<TImpl, TBases...>
    {
    public:
        SimpleFaceLayout(FakeConstructionArg fc)
            : TBases(fc)...
        {
            (void)fc;
        }
    };

}
