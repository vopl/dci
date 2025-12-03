// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
