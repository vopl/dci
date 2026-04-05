/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <cstddef>
#include <type_traits>

namespace dci::himpl::details
{
    template <class T, std::size_t padSizeTest, bool=std::is_final<T>::value>
    struct PadEvaluatorTest : T
    {
        char _pad[padSizeTest];
    };

    template <class T, std::size_t padSizeTest>
    struct PadEvaluatorTest<T, padSizeTest, true>
    {
        alignas(T) char _v[sizeof(T)];
        char _pad[padSizeTest];
    };


    template <class T, std::size_t padSizeTest=0, bool stop=(sizeof(PadEvaluatorTest<T, padSizeTest+1>) > sizeof(T))>
    struct PadEvaluator
    {
        static const std::size_t _size = PadEvaluator<T, padSizeTest+1>::_size;
    };

    template <class T, std::size_t padSizeTest>
    struct PadEvaluator<T, padSizeTest, true>
    {
        static const std::size_t _size = padSizeTest;
    };


    std::false_type TestTryDestructionCall(...);

    template <class T>
    std::true_type TestTryDestructionCall(T*, decltype(std::declval<T*>()->tryDestruction(std::declval<T*>())) * = nullptr);




    template <class T>
    struct MetaInfoFetcher
    {
        static constexpr std::size_t _size = sizeof(T) - PadEvaluator<T>::_size;
        static constexpr std::size_t _align = alignof(T);

        static constexpr bool _isPolymorphic            = std::is_polymorphic<T>::value;

        static constexpr bool _hasDefaultCtor           = std::is_default_constructible<T>::value;
        static constexpr bool _hasNothrowDefaultCtor    = std::is_nothrow_default_constructible<T>::value;

        static constexpr bool _hasCopyCtor              = std::is_copy_constructible<T>::value;
        static constexpr bool _hasNothrowCopyCtor       = std::is_nothrow_copy_constructible<T>::value;
        static constexpr bool _hasMoveCtor              = std::is_move_constructible<T>::value;
        static constexpr bool _hasNothrowMoveCtor       = std::is_nothrow_move_constructible<T>::value;

        static constexpr bool _hasCopyAssign            = std::is_copy_assignable<T>::value;
        static constexpr bool _hasNothrowCopyAssign     = std::is_nothrow_copy_assignable<T>::value;
        static constexpr bool _hasMoveAssign            = std::is_move_assignable<T>::value;
        static constexpr bool _hasNothrowMoveAssign     = std::is_nothrow_move_assignable<T>::value;


        static constexpr bool _isAbstract               = std::is_abstract<T>::value;

        static constexpr bool _isFinal                  = std::is_final<T>::value;
        static constexpr bool _hasDestructor            = std::is_destructible<T>::value;
        static constexpr bool _hasNothrowDestructor     = std::is_nothrow_destructible<T>::value;
        static constexpr bool _hasDestructorCaller      = decltype(TestTryDestructionCall(std::declval<T*>()))::value;

        template <class F>
        static void enumerateBases(F){}

        template <class F, class TFirstBaseCandidate, class... TOtherBaseCandidates>
            requires(std::is_base_of<TFirstBaseCandidate, T>::value && !std::is_same<TFirstBaseCandidate, T>::value)
        static void enumerateBases(F f)
        {
            f(static_cast<TFirstBaseCandidate*>(nullptr));
            enumerateBases<F, TOtherBaseCandidates...>(f);
        }

        template <class F, class TFirstBaseCandidate, class... TOtherBaseCandidates>
            requires(!(std::is_base_of<TFirstBaseCandidate, T>::value && !std::is_same<TFirstBaseCandidate, T>::value))
        static void enumerateBases(F f)
        {
            enumerateBases<F, TOtherBaseCandidates...>(f);
        }


    };

}
