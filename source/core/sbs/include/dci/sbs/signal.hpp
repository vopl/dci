// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "wire/callback.hpp"
#include "wire/transfer.hpp"
#include "box.hpp"
#include "owner.hpp"

namespace dci::sbs
{
    template <class R=void, class... Args>
    class Signal
    {
    public:
        Signal() = default;
        Signal(Box& box);
        Signal(const Signal&) = default;
        Signal(Signal&&) = default;
        ~Signal();

        Signal& operator=(const Signal&) = default;
        Signal& operator=(Signal&&) = default;

        template <class F>
        void connect(F&& f) requires wire::Callback<R, F, Args...>::_valid;

        template <class F>
        void connect(Owner& owner, F&& f) requires wire::Callback<R, F, Args...>::_valid;

    private:
        Box* _box{};
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    Signal<R, Args...>::Signal(sbs::Box& box)
        : _box(&box)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    Signal<R, Args...>::~Signal()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    template <class F>
    void Signal<R, Args...>::connect(F&& f) requires wire::Callback<R, F, Args...>::_valid
    {
        Subscription* s = new wire::Callback<R, F, Args...>{nullptr, std::forward<F>(f)};

        _box->push(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, class... Args>
    template <class F>
    void Signal<R, Args...>::connect(Owner& owner, F&& f) requires wire::Callback<R, F, Args...>::_valid
    {
        Subscription* s = new wire::Callback<R, F, Args...>{&owner, std::forward<F>(f)};

        _box->push(s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR=void, class... SArgs, class F>
    void operator+=(Signal<SR, SArgs...>&& signal, F&& f)
    {
        signal.connect(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR=void, class... SArgs, class F>
    void operator+=(Signal<SR, SArgs...>& signal, F&& f)
    {
        signal.connect(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR=void, class... SArgs, class F>
    void operator+=(Signal<SR, SArgs...>&& signal, OwnedFunctor<F>&& of)
    {
        signal.connect(of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR=void, class... SArgs, class F>
    void operator+=(Signal<SR, SArgs...>&& signal, OwnedFunctor<F>& of)
    {
        signal.connect(of._owner, of._f);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR=void, class... SArgs, class F>
    void operator+=(Signal<SR, SArgs...>& signal, OwnedFunctor<F>&& of)
    {
        signal.connect(of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR=void, class... SArgs, class F>
    void operator+=(Signal<SR, SArgs...>& signal, OwnedFunctor<F>& of)
    {
        signal.connect(of._owner, of._f);
    }
}
