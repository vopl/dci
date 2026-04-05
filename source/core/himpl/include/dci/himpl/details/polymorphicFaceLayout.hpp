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
#include "../check/isFirstPolymorphic.hpp"
#include "metaInfoFetcher.hpp"
#include "space.hpp"

#include <algorithm>

namespace dci::himpl::details
{
    //суффикс для полиморфного случая. Размер - от размера текущего импла отнять (размер полиморфной области + размер базовых классов)
    template <class... TBases>
    struct BasesSizeProber
        : public TBases...
    {
    };

    template <class... TBases>
    struct BasesSizeEvaluator
    {
        using Prober = BasesSizeProber<TBases...>;
        static constexpr std::size_t _value = MetaInfoFetcher<Prober>::_size - (std::is_polymorphic<Prober>::value ? sizeof(void*) : 0);
    };

    template <class TImpl, class... TBases>
    struct AlignedImplSizeEvaluator
    {
        static const std::size_t _align = std::max(alignof(void*), std::max(ImplMetaInfo<TImpl>::_align, alignof(BasesSizeProber<TBases...>)));
        struct alignas(_align) Prober
        {
            char _space[ImplMetaInfo<TImpl>::_size];
        };
        static constexpr std::size_t _value = MetaInfoFetcher<Prober>::_size;
    };


    template <class TImpl, class... TBases>
    class PolymorphicAreaSuffix
        : public Space<
            std::integral_constant<std::size_t,  AlignedImplSizeEvaluator<TImpl>::_value - BasesSizeEvaluator<TBases...>::_value>,
            PolymorphicAreaSuffix<TImpl, TBases...>>
    {
    };











    //есть полиморфность
    template <class TImpl, class... TBases>
    class PolymorphicFaceLayout
        : public TBases...
        , private PolymorphicAreaSuffix<TImpl, TBases...>
    {
    private:
        struct VTableProbe
        {
            void* _vTablePtr;
            alignas(PolymorphicFaceLayout) std::byte x[sizeof(PolymorphicFaceLayout)];
        };

    public:
        static constexpr std::size_t _implOffset = offsetof(VTableProbe, x);

    public:
        PolymorphicFaceLayout(FakeConstructionArg fc)
            : TBases(fc)...
        {
            (void)fc;
        }
    };


}
