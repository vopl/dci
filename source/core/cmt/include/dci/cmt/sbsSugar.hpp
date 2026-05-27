/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "task/owner.hpp"
#include "functions.hpp"
#include <dci/sbs/signal.hpp>

namespace dci::cmt::sbsSugar
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct Owned2
    {
        dci::sbs::Owner&    _sbsOwner;
        cmt::task::Owner&   _cmtOwner;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Owned2 operator*(dci::sbs::Owner& sbsOwner, cmt::task::Owner& cmtOwner)
    {
        return {sbsOwner, cmtOwner};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    inline Owned2 operator*(cmt::task::Owner& cmtOwner, dci::sbs::Owner& sbsOwner)
    {
        return {sbsOwner, cmtOwner};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    struct Owned2Functor : Owned2
    {
        F&& _f;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    Owned2Functor<F> operator*(dci::sbs::Owner& sbsOwner, cmt::task::OwnedFunctor<F>& of)
    {
        return {{sbsOwner, of._owner}, std::forward<F>(of._f)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    Owned2Functor<F> operator*(cmt::task::Owner& cmtOwner, dci::sbs::OwnedFunctor<F>& of)
    {
        return {{of._owner, cmtOwner}, std::forward<F>(of._f)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class F>
    Owned2Functor<F> operator*(Owned2&& o2, F&& f)
    {
        return {o2, std::forward<F>(f)};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... SArgs, class F>
    void connect(dci::sbs::Signal<void, SArgs...>& signal, dci::sbs::Owner* sbsOwner, cmt::task::Owner* cmtOwner, F&& f) requires dci::sbs::wire::Callback<void, F, SArgs...>::_valid
    {
        signal.connect(sbsOwner, [cmtOwner, f{std::forward<F>(f)}](auto&&... args) -> void
        {
            dci::cmt::spawn(cmtOwner, [f{std::move(f)}, ...args{std::forward<decltype(args)>(args)}] mutable
            {
                f(std::move(args)...);
            });
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR, class... SArgs, class F>
    void connect(dci::sbs::Signal<cmt::Future<SR>, SArgs...>& signal, dci::sbs::Owner* sbsOwner, cmt::task::Owner* cmtOwner, F&& f) requires dci::sbs::wire::Callback<SR, F, SArgs...>::_valid
    {
        signal.connect(sbsOwner, [cmtOwner, f{std::forward<F>(f)}](auto&&... args) -> cmt::Future<SR>
        {
            return dci::cmt::spawnv(cmtOwner, [f{std::move(f)}, ...args{std::forward<decltype(args)>(args)}] mutable
            {
                return f(std::move(args)...);
            });
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... SArgs, class F>
    void operator+=(dci::sbs::Signal<void, SArgs...>&& signal, Owned2Functor<F>&& o2f) requires dci::sbs::wire::Callback<void, F, SArgs...>::_valid
    {
        return connect(signal, &o2f._sbsOwner, &o2f._cmtOwner, std::forward<F>(o2f._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... SArgs, class F>
    void operator+=(dci::sbs::Signal<void, SArgs...>& signal, Owned2Functor<F>&& o2f) requires dci::sbs::wire::Callback<void, F, SArgs...>::_valid
    {
        return connect(signal, &o2f._sbsOwner, &o2f._cmtOwner, std::forward<F>(o2f._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... SArgs, class F>
    void operator+=(dci::sbs::Signal<void, SArgs...>&& signal, cmt::task::OwnedFunctor<F>&& of) requires dci::sbs::wire::Callback<void, F, SArgs...>::_valid
    {
        return connect(signal, nullptr, &of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... SArgs, class F>
    void operator+=(dci::sbs::Signal<void, SArgs...>& signal, cmt::task::OwnedFunctor<F>&& of) requires dci::sbs::wire::Callback<void, F, SArgs...>::_valid
    {
        return connect(signal, nullptr, &of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR, class... SArgs, class F>
    void operator+=(dci::sbs::Signal<cmt::Future<SR>, SArgs...>&& signal, Owned2Functor<F>&& o2f) requires dci::sbs::wire::Callback<SR, F, SArgs...>::_valid
    {
        return connect(signal, &o2f._sbsOwner, &o2f._cmtOwner, std::forward<F>(o2f._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR, class... SArgs, class F>
    void operator+=(dci::sbs::Signal<cmt::Future<SR>, SArgs...>& signal, Owned2Functor<F>&& o2f) requires dci::sbs::wire::Callback<SR, F, SArgs...>::_valid
    {
        return connect(signal, &o2f._sbsOwner, &o2f._cmtOwner, std::forward<F>(o2f._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR, class... SArgs, class F>
    void operator+=(dci::sbs::Signal<cmt::Future<SR>, SArgs...>&& signal, cmt::task::OwnedFunctor<F>&& of) requires dci::sbs::wire::Callback<SR, F, SArgs...>::_valid
    {
        return connect(signal, nullptr, &of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class SR, class... SArgs, class F>
    void operator+=(dci::sbs::Signal<cmt::Future<SR>, SArgs...>& signal, cmt::task::OwnedFunctor<F>&& of) requires dci::sbs::wire::Callback<SR, F, SArgs...>::_valid
    {
        return connect(signal, nullptr, &of._owner, std::forward<F>(of._f));
    }
}

namespace dci::sbs
{
    using dci::cmt::sbsSugar::operator*;
    using dci::cmt::sbsSugar::operator+=;
}

namespace dci::cmt::task
{
    using dci::cmt::sbsSugar::operator*;
}
