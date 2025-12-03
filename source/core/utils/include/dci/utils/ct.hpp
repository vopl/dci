// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstddef>
#include <utility>
#include <array>
#include <algorithm>

namespace dci::utils::ct
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <auto v>
    struct Value
    {
        using UT = decltype(v);
        template <UT v2> using Rebind = Value<v2>;
        static constexpr const UT _v = v;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... T>
    struct TList;

    template <auto... v>
    using VList = TList<Value<v>...>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class...>
        struct TListGrepImpl;

        template <class... Already>
        struct TListGrepImpl<TList<Already...>>
        {
            using Result = TList<Already...>;
        };

        template <class... Already, class Next, auto positiveCondition, class... Tail>
        requires (static_cast<bool>(positiveCondition))
        struct TListGrepImpl<TList<Already...>, TList<Next, Value<positiveCondition>>, Tail...>
        {
            using Result = typename TListGrepImpl<TList<Already..., Next>, Tail...>::Result;
        };

        template <class... Already, class Unclaimed, class... Tail>
        struct TListGrepImpl<TList<Already...>, Unclaimed, Tail...>
        {
            using Result = typename TListGrepImpl<TList<Already...>, Tail...>::Result;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <std::size_t idx, class... T>
        struct TListGetImpl;

        template <class T0, class... TTail>
        struct TListGetImpl<0, T0, TTail...>
        {
            using Result = T0;
        };

        template <std::size_t idx, class T0, class... TTail>
        struct TListGetImpl<idx, T0, TTail...>
        {
            using Result = typename TListGetImpl<idx-1, TTail...>::Result;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class... T>
        struct TListPopFrontImpl;

        template <class T0, class... TTail>
        struct TListPopFrontImpl<T0, TTail...>
        {
            using Result = TList<TTail...>;
        };

        template <>
        struct TListPopFrontImpl<>
        {
            using Result = TList<>;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class...>
        struct TListPopBackImpl;

        template <class... TFront, class TNext, class... TTail>
        struct TListPopBackImpl<TList<TFront...>, TNext, TTail...>
        {
            using Result = typename TListPopBackImpl<TList<TFront..., TNext>, TTail...>::Result;
        };

        template <class... TFront, class TBack>
        struct TListPopBackImpl<TList<TFront...>, TBack>
        {
            using Result = TList<TFront...>;
        };

        template <class... TFront>
        struct TListPopBackImpl<TList<TFront...>>
        {
            using Result = TList<TFront...>;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <template <class...> class Combiner, class...>
        struct TListZipImpl;

        template <template <class...> class Combiner, class... A, class... B>
        struct TListZipImpl<Combiner, TList<A...>, TList<B...>>
        {
            using Result = TList<Combiner<A, B>...>;
        };

        template <template <class...> class Combiner, class... A, class... B, class... C>
        struct TListZipImpl<Combiner, TList<A...>, TList<B...>, TList<C...>>
        {
            using Result = TList<Combiner<A, B, C>...>;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class...>
        struct TListLinearizeImpl;

        template <class... Already>
        struct TListLinearizeImpl<TList<Already...>>
        {
            using Result = TList<Already...>;
        };

        template <class... Already, class Next, class... Tail>
        struct TListLinearizeImpl<TList<Already...>, Next, Tail...>
        {
            using Result = typename TListLinearizeImpl<TList<Already..., Next>, Tail...>::Result;
        };

        template <class... Already, class... Next, class... Tail>
        struct TListLinearizeImpl<TList<Already...>, TList<Next...>, Tail...>
        {
            using Result = typename TListLinearizeImpl<typename TList<Already...>::template Append<typename TList<Next...>::template Linearize<>>, Tail...>::Result;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <class...>
        struct TListUniqueImpl;

        template <class Already>
        struct TListUniqueImpl<Already>
        {
            using Result = Already;
        };

        template <class Already, class Next, class... Tail>
        requires (Already::template _contains<Next>)
        struct TListUniqueImpl<Already, Next, Tail...>
        {
            using Result = typename TListUniqueImpl<Already, Tail...>::Result;
        };

        template <class Already, class Next, class... Tail>
        requires (!Already::template _contains<Next>)
        struct TListUniqueImpl<Already, Next, Tail...>
        {
            using Result = typename TListUniqueImpl<typename Already::template PushBack<Next>, Tail...>::Result;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... T>
    struct TList
    {
        static constexpr const std::size_t _size = sizeof...(T);

        template <class U>
        static constexpr const bool _contains = (false || ... || std::is_same_v<T, U>);

        template <class U>
        requires (_contains<U>)
        static constexpr const std::size_t _index = []
        {
            std::size_t idx{};
            bool found{};
            ((std::is_same_v<T, U> ? (found = true) : (found ? 0 : ++idx)), ...);
            return idx;
        }();

        template <auto idx>
        requires (static_cast<std::size_t>(idx) < _size)
        using Get = typename details::TListGetImpl<static_cast<std::size_t>(idx), T...>::Result;

        template <class... T2>
        using PushFront = TList<T2..., T...>;

        template <class... T2>
        using PushBack = TList<T..., T2...>;

        template <class=void>
        using PopFront = typename details::TListPopFrontImpl<T...>::Result;

        template <class=void>
        using PopBack = typename details::TListPopBackImpl<TList<>, T...>::Result;

        template <class L>
        using Append = typename L::template PushFront<T...>;

        template <class L>
        using Prepend = typename L::template PushBack<T...>;

        template <template <class> class Predicate>
        using Grep = typename details::TListGrepImpl<TList<>, TList<T, Predicate<T>>...>::Result;

        template <template <class> class Manipula>
        using Map = TList<Manipula<T>...>;

        template <template <class...> class Combiner, class... WithList>
        using Zip = typename details::TListZipImpl<Combiner, TList<T...>, WithList...>::Result;

        template <class=void>
        using Linearize = typename details::TListLinearizeImpl<TList<>, T...>::Result;

        template <class=void>
        using Unique = typename details::TListUniqueImpl<TList<>, T...>::Result;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace details
    {
        template <auto N>
        constexpr auto makeSeq()
        {
            if constexpr(!N)
                return VList<>{};
            else
                return []<decltype(N)... half>(VList<half...>) constexpr
                {
                    if constexpr(N&1)
                        return VList<half..., (N/2+half)..., N-1>{};
                    else
                        return VList<half..., (N/2+half)...>{};
                }(makeSeq<N/2>());
        }
    }

    template <auto N>
    using MakeSeq = decltype(details::makeSeq<N>());

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <auto b, class T, class F>
    using If = std::conditional_t<static_cast<bool>(b), T, F>;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    using GetResult = typename T::Result;
}

namespace dci::utils::ct
{
    template <typename Type, std::size_t... sizes>
    constexpr auto concatenate(const std::array<Type, sizes>&... arrays)
    {
        std::array<Type, (sizes + ...)> result;

        std::size_t index{};
        ((std::copy_n(arrays.begin(), sizes, result.begin() + index), index += sizes), ...);

        return result;
    }
}
