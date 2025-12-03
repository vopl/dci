// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "transfer.hpp"
#include <dci/sbs/subscription.hpp>
#include <dci/mm/heap/allocable.hpp>

namespace dci::sbs::wire
{
    template <class R, class F, class... Args>
    class Callback
        : public sbs::Subscription
        , public mm::heap::Allocable<Callback<R, F, Args...>>
    {
    public:
        Callback(sbs::Owner* owner, F&& f);

    private:
        static void activator(sbs::Subscription* bs, void* context, std::uint_fast8_t flags);
        auto invoke(const std::tuple<transfer::Arg<Args>...>& args, bool isLast);

    public:
        constexpr static const bool _valid = std::is_same_v<R, decltype(std::declval<Callback>().invoke(std::declval<std::tuple<transfer::Arg<Args>...>>(), true))>;

    private:
         template <class... TargetArgs>
         struct Applyer;

    private:
        std::decay_t<F> _f;
    };


    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class F, class... Args>
    Callback<R, F, Args...>::Callback(sbs::Owner* owner, F&& f)
        : sbs::Subscription(&Callback::activator, owner)
        , _f(std::forward<F>(f))
    {}

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class F, class... Args>
    void Callback<R, F, Args...>::activator(sbs::Subscription* bs, void* context, std::uint_fast8_t flags)
    {
        Callback * self = static_cast<Callback*>(bs);

        if(sbs::Subscription::act & flags)
        {
            bool isLast = flags & sbs::Subscription::act4Last;
            TransferBase* tb = static_cast<TransferBase *>(context);

            switch(tb->_agg)
            {
            case Agg::first:
                {
                    using T = Transfer<R, Agg::first, Args...>;
                    T* t = static_cast<T*>(tb);

                    if constexpr (!std::is_same_v<void, R>)
                    {
                        if(!t->_ret._assigned)
                        {
                            t->_ret._value = self->invoke(t->_args, isLast);
                            t->_ret._assigned = true;
                        }
                        else
                        {
                            self->invoke(t->_args, isLast);
                        }
                    }
                    else
                    {
                        self->invoke(t->_args, isLast);
                    }
                }
                break;

            case Agg::last:
                {
                    using T = Transfer<R, Agg::last, Args...>;
                    T* t = static_cast<T*>(tb);

                    if constexpr (!std::is_same_v<void, R>)
                    {
                        t->_ret._value = self->invoke(t->_args, isLast);
                        t->_ret._assigned = true;
                    }
                    else
                    {
                        self->invoke(t->_args, isLast);
                    }
                }
                break;

            case Agg::all:
                {
                    using T = Transfer<R, Agg::all, Args...>;
                    T* t = static_cast<T*>(tb);

                    if constexpr (!std::is_same_v<void, R>)
                    {
                        t->_ret._value.emplace_back(self->invoke(t->_args, isLast));
                    }
                    else
                    {
                        self->invoke(t->_args, isLast);
                    }
                }
                break;
            }
        }

        if(sbs::Subscription::del & flags)
        {
            delete self;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class F, class... Args>
    auto Callback<R, F, Args...>::invoke(const std::tuple<transfer::Arg<Args>...>& args, bool isLast)
    {
        return [&]<std::size_t... I>(std::index_sequence<I...>)
        {
            if(isLast)
            {
                return Applyer<>::exec(_f, std::get<I>(args)...);
            }

            return Applyer<>::exec(_f,
                        transfer::Arg<
                            std::remove_reference_t<
                                decltype(std::get<I>(args).rr())
                            >
                        >(std::get<I>(args).cr())...);
        }(std::make_index_sequence<sizeof...(Args)>{});
    }

    namespace callback
    {
        struct BadResult{};
    }

    template <class R, class F, class... Args>
    template <class... Adapted>
    struct Callback<R, F, Args...>::Applyer
    {
        template <std::size_t processedInRuntime=0, class... Original>
        static auto exec(F&, Adapted&&..., const transfer::Arg<Original>&...)
        {
            return callback::BadResult();
        }

        template <std::size_t processedInRuntime=0>
        static auto exec(F& f, Adapted&&... adapted)
        requires(std::is_invocable_v<F&, Adapted&&...>)
        {
            return f(std::forward<Adapted>(adapted)...);
        }
        static constexpr std::size_t _maxPrioriteArgs = 10;

        template <std::size_t processedInRuntime=0, class OriginalHead, class... OriginalTail>
        static auto exec(F& f, Adapted&&... adapted, const transfer::Arg<OriginalHead>& transferHead, const transfer::Arg<OriginalTail>&... transferTail)
        {
            constexpr bool can_cr = std::is_invocable_v<F&, Adapted&&..., const OriginalHead& , OriginalTail&&...>;
            constexpr bool can_rr = std::is_invocable_v<F&, Adapted&&...,       OriginalHead&&, OriginalTail&&...>;

            if constexpr(!can_rr && !can_cr)
            {
                return callback::BadResult();
            }
            else if constexpr(can_rr && !can_cr)
            {
                return Applyer<Adapted..., OriginalHead&&>::template
                       exec<processedInRuntime>(
                            f,
                            std::forward<Adapted>(adapted)...,
                            transferHead.rr(),
                            transferTail...);
            }
            else if constexpr(can_cr && !can_rr)
            {
                return Applyer<Adapted..., const OriginalHead&>::template
                       exec<processedInRuntime>(
                            f,
                            std::forward<Adapted>(adapted)...,
                            transferHead.cr(),
                            transferTail...);
            }
            else
            {
                if constexpr(_maxPrioriteArgs >= (processedInRuntime+1))
                {
                    if constexpr(transfer::Arg<OriginalHead>::_preferConstAllways)
                    {
                        return Applyer<Adapted..., const OriginalHead&>::template
                               exec<processedInRuntime>(
                                    f,
                                    std::forward<Adapted>(adapted)...,
                                    transferHead.cr(),
                                    transferTail...);
                    }
                    else
                    {
                        if(transferHead.preferConst())
                        {
                            return Applyer<Adapted..., const OriginalHead&>::template
                                   exec<processedInRuntime+1>(
                                        f,
                                        std::forward<Adapted>(adapted)...,
                                        transferHead.cr(),
                                        transferTail...);
                        }
                        else
                        {
                            return Applyer<Adapted..., OriginalHead&&>::template
                                   exec<processedInRuntime+1>(
                                        f,
                                        std::forward<Adapted>(adapted)...,
                                        transferHead.rr(),
                                        transferTail...);
                        }
                    }
                }
                else
                {
                    return Applyer<Adapted..., const OriginalHead&>::template
                           exec<processedInRuntime>(
                                f,
                                std::forward<Adapted>(adapted)...,
                                transferHead.cr(),
                                transferTail...);
                }
            }
        }
    };
}
