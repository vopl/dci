/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <utility>
#include <type_traits>

namespace dci::cmt::details::transformHolder
{
    template <class F, class SourceStream, template<class...> class TDestinationStream>
    struct DestinationDetector
    {
        struct BadResult {};

        template <class... Args>
        struct DetectFromLastArg
        {
            using Result = BadResult;
        };

        template <class Dest>
        struct DetectFromLastArg<TDestinationStream<Dest>>
        {
            using Result = Dest;
        };

        template <class Last>
        struct DetectFromLastArg<Last> : DetectFromLastArg<std::decay_t<Last>> {};

        template <class ArgsHead, class... ArgsTail>
        struct DetectFromLastArg<ArgsHead, ArgsTail...> : DetectFromLastArg<ArgsTail...> {};


        template <class... PassedArgs>
        struct DetectFromFunctor
        {
            template <         class... Args> static auto test(void (   *)(Args...)               ) -> typename DetectFromLastArg<Args...>::Result;
            template <class C, class... Args> static auto test(void (C::*)(Args...)               ) -> typename DetectFromLastArg<Args...>::Result;
            template <class C, class... Args> static auto test(void (C::*)(Args...) const         ) -> typename DetectFromLastArg<Args...>::Result;
            template <class C, class... Args> static auto test(void (C::*)(Args...) volatile      ) -> typename DetectFromLastArg<Args...>::Result;
            template <class C, class... Args> static auto test(void (C::*)(Args...) const volatile) -> typename DetectFromLastArg<Args...>::Result;

            static auto test(...) requires std::is_invocable_v<F, PassedArgs...>
            {
                return std::invoke_result_t<F, PassedArgs...>();
            }

            template <class C, class Res = decltype(test(&C::operator()))>
            static auto test(C&&) -> Res;

            static auto test(...) -> BadResult;

            using Result = decltype(test(std::forward<F>(std::declval<F>())));
        };

        constexpr static auto test()
        {
            using Result1 = typename DetectFromFunctor<>::Result;

            if constexpr(!std::is_same_v<BadResult, Result1>)
            {
                return Result1();
            }
            else if constexpr(std::is_same_v<void, SourceStream>)
            {
                return;//void
            }
            else
            {
                using Result2 = typename DetectFromFunctor<SourceStream>::Result;
                if constexpr(!std::is_same_v<BadResult, Result2>)
                {
                    return Result2();
                }
                else
                {
                    using Result3 = typename DetectFromFunctor<SourceStream&&>::Result;
                    if constexpr(!std::is_same_v<BadResult, Result3>)
                    {
                        return Result3();
                    }
                    else
                    {
                        using Result4 = typename DetectFromFunctor<SourceStream&>::Result;
                        if constexpr(!std::is_same_v<BadResult, Result4>)
                        {
                            return Result4();
                        }
                        else
                        {
                            return;
                        }
                    }
                }
            }
        }

        using Result = decltype(test());
    };
}
