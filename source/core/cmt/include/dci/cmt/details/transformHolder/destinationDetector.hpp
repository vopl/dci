// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
