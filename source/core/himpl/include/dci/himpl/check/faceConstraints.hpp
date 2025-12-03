// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../implMetaInfo.hpp"

#include <type_traits>
#include <cstddef>

namespace dci::himpl::check
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... T>
    struct NoPolymorphicsAfterFirstEvaluator
    {
        static const bool _value = true;
    };

    template <class TFirstBase, class... TOtherBases>
    struct NoPolymorphicsAfterFirstEvaluator<TFirstBase, TOtherBases...>
    {
        static const bool _value = (!std::is_polymorphic<TOtherBases>::value && ... );
    };

    template<class... TBases>
    concept NoPolymorphicsAfterFirst = check::NoPolymorphicsAfterFirstEvaluator<TBases...>::_value;


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class... TBases>
    concept NoFinals = (!std::is_final<TBases>::value && ... );

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T, class... Args>
    struct IsConstructibleFromDerived
    {
        struct DerivedStub
            : public T
        {
            template <class, class...>
            friend struct IsConstructibleFromDerived;
            using T::T;
        };

        template <class DS> static std::true_type  test(decltype(new DS{std::declval<Args>()...})*);
        template <class DS> static std::false_type test(...);

        static constexpr bool value = decltype(test<DerivedStub>(nullptr))::value;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    struct IsDestructibleFromDerived
    {
        struct DerivedStub
            : public T
        {
            ~DerivedStub();
        };

        static constexpr bool value = std::is_destructible<DerivedStub>::value;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class TBase>
    concept BaseFace =
        IsConstructibleFromDerived<TBase, FakeConstructionArg>::value &&
        IsDestructibleFromDerived<TBase>::value &&
        requires
        {
            typename TBase::Impl;
            typename TBase::Face;
            typename TBase::BasesTuple;
        };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class TImpl>
    concept CanBeDestructed = ImplMetaInfo<TImpl>::_hasDestructorCaller ||
                                   (ImplMetaInfo<TImpl>::_isFinal && 0 == std::tuple_size<typename ImplMetaInfo<TImpl>::BasesTuple>::value);



    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template<class TImpl>
    constexpr bool InheritanceSymmetryEvaluator(std::size_t currentOffset)
    {
        (void)currentOffset;
        return true;
    }

    template<class TImpl, class TNextBase, class... TOtherBases>
    constexpr bool InheritanceSymmetryEvaluator(std::size_t currentOffset)
    {
        constexpr std::size_t nextOffset = ImplMetaInfo<TImpl>::template baseOffset<typename TNextBase::Impl>();
        return
                ~std::size_t(0) > nextOffset &&
                currentOffset <= nextOffset &&
                InheritanceSymmetryEvaluator<TImpl, TOtherBases...>(nextOffset);
    }

    template<class TImpl, class... TBases>
    concept IsInheritanceSymmetric = InheritanceSymmetryEvaluator<TImpl, TBases...>(0);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class TFace, class TImpl, class... TBases>
    concept FaceLayoutParams = CanBeDestructed<TImpl> &&
                               IsInheritanceSymmetric<TImpl, TBases...> && // Impl наследует все имплы от баз и именно в таком порядке
                               NoPolymorphicsAfterFirst<TBases...> &&
                               NoFinals<TBases...> &&
                               (BaseFace<TBases> && ...);
}
