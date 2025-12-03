// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "signal.hpp"
#include "wire/transfer.hpp"
#include <dci/sbs/box.hpp>

namespace dci::sbs
{

    template <class R=void, class... Args> class Wire
    {
    public:

        Wire();
        ~Wire();

        template <wire::Agg agg=wire::Agg::first, class... RawArgs>
        auto in(RawArgs&&... rawArgs);

        template <wire::Agg agg=wire::Agg::first, class... RawArgs>
        auto in2(auto&& retAdapter, RawArgs&&... rawArgs);

        Signal<R, Args...> out();

        bool connected();
        void disconnectAll();

    private:
        Box _box;
    };






    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    Wire<R, Args...>::Wire()
    {

    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    Wire<R, Args...>::~Wire()
    {

    }


    namespace
    {
        template <class... T>
        struct Pack;

        template <class AdaptedPack, class RawPack, class NeedPack>
        struct ArgsAdapter;

        template <class... Adapted>
        struct ArgsAdapter<Pack<Adapted...>, Pack<>, Pack<>>
        {
            template <class F>
            static auto exec(F&& f, Adapted&&... adapted)
            {
                return f(std::forward<Adapted>(adapted)...);
            }
        };

        template <class... Adapted, class RawHead, class... RawTail, class NeedHead, class... NeedTail>
        struct ArgsAdapter<Pack<Adapted...>, Pack<RawHead, RawTail...>, Pack<NeedHead, NeedTail...>>
        {
            template <class F>
            static auto exec(F&& f, Adapted&&... adapted, RawHead&& rawHead, RawTail&&... rawTail)
            {
                if constexpr(std::is_same_v<std::decay_t<RawHead>, std::decay_t<NeedHead>>)
                {
                    return ArgsAdapter<
                                Pack<Adapted..., RawHead>,
                                Pack<RawTail...>,
                                Pack<NeedTail...>>::
                           exec(
                                std::forward<F>(f),
                                std::forward<Adapted>(adapted)...,
                                std::forward<RawHead>(rawHead),
                                std::forward<RawTail>(rawTail)...);
                }
                else
                {
                    return ArgsAdapter<
                                Pack<Adapted..., NeedHead>,
                                Pack<RawTail...>,
                                Pack<NeedTail...>>::
                           exec(
                                std::forward<F>(f),
                                std::forward<Adapted>(adapted)...,
                                NeedHead(std::forward<RawHead>(rawHead)),
                                std::forward<RawTail>(rawTail)...);
                }
            }
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    template <wire::Agg agg, class... RawArgs>
    auto Wire<R, Args...>::in(RawArgs&&... rawArgs)
    {
        return in2<agg>([](wire::transfer::Ret<R, agg>& res)
        {
            return res.detach();
        }, std::forward<RawArgs>(rawArgs)...);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    template <wire::Agg agg, class... RawArgs>
    auto Wire<R, Args...>::in2(auto&& retAdapter, RawArgs&&... rawArgs)
    {
        return ArgsAdapter<Pack<>, Pack<RawArgs...>, Pack<Args...>>::exec([&](auto&&...adaptedArgs)
        {
            wire::Transfer<R, agg, Args...> transfer(std::forward<decltype(adaptedArgs)>(adaptedArgs)...);
            _box.activate(&transfer, 0);
            return retAdapter(transfer._ret);
        }, std::forward<RawArgs>(rawArgs)...);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    Signal<R, Args...> Wire<R, Args...>::out()
    {
        return Signal<R, Args...>(_box);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    bool Wire<R, Args...>::connected()
    {
        return !_box.empty();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    void Wire<R, Args...>::disconnectAll()
    {
        _box.removeAndDeleteAll();
    }

}
