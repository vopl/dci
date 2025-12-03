// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
