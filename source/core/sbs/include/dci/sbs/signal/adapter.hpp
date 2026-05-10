/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "../owner.hpp"

namespace dci::sbs
{
    template <class R, class... Args>
    class Signal;
}

namespace dci::sbs::signal
{
    template <class Transform, class SR, class... SArgs>
    class Adapter
    {
    public:
        Adapter(Transform&& transform, Signal<SR, SArgs...>&& signal);
        Adapter(const Adapter&) = delete;
        Adapter(Adapter&&) = delete;
        ~Adapter();

        Adapter& operator=(const Adapter&) = delete;
        Adapter& operator=(Adapter&&) = delete;

        template <class F>
        void connect(F&& f);

        template <class F>
        void connect(Owner& owner, F&& f);

    private:
        std::decay_t<Transform> _transform;
        Signal<SR, SArgs...>    _signal;
    };

    template <class Transform, class SR, class... SArgs>
    Adapter(Transform&&, Signal<SR, SArgs...>&&) -> Adapter<Transform, SR, SArgs...>;
}

namespace dci::sbs::signal
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs>
    Adapter<Transform, SR, SArgs...>::Adapter(Transform&& transform, Signal<SR, SArgs...>&& signal)
        : _transform{std::forward<Transform>(transform)}
        , _signal{std::move(signal)}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs>
    Adapter<Transform, SR, SArgs...>::~Adapter()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs>
    template <class F>
    void Adapter<Transform, SR, SArgs...>::connect(F&& f)
    {
        _signal.connect([transform=_transform, f{std::forward<F>(f)}](auto&&... args)
        {
            return transform(f, std::forward<decltype(args)>(args)...);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs>
    template <class F>
    void Adapter<Transform, SR, SArgs...>::connect(Owner& owner, F&& f)
    {
        _signal.connect(owner, [transform=_transform, f{std::forward<F>(f)}](auto&&... args)
        {
            return transform(f, std::forward<decltype(args)>(args)...);
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs, class F>
    void operator+=(Adapter<Transform, SR, SArgs...>&& adapter, F&& f)
    {
        adapter.connect(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs, class F>
    void operator+=(Adapter<Transform, SR, SArgs...>& adapter, F&& f)
    {
        adapter.connect(std::forward<F>(f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs, class F>
    void operator+=(Adapter<Transform, SR, SArgs...>&& adapter, OwnedFunctor<F>&& of)
    {
        adapter.connect(of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs, class F>
    void operator+=(Adapter<Transform, SR, SArgs...>&& adapter, OwnedFunctor<F>& of)
    {
        adapter.connect(of._owner, of._f);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs, class F>
    void operator+=(Adapter<Transform, SR, SArgs...>& adapter, OwnedFunctor<F>&& of)
    {
        adapter.connect(of._owner, std::forward<F>(of._f));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Transform, class SR, class... SArgs, class F>
    void operator+=(Adapter<Transform, SR, SArgs...>& adapter, OwnedFunctor<F>& of)
    {
        adapter.connect(of._owner, of._f);
    }
}
