/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/future.hpp>
#include <dci/cmt/promise.hpp>
#include <dci/sbs/signal.hpp>

namespace dci::utils
{
    namespace s2f
    {
        template <class... Args> struct TransitImpl      { using Result = std::tuple<Args...>; };
        template <class Arg>     struct TransitImpl<Arg> { using Result = Arg; };
        template <>              struct TransitImpl<>    { using Result = void; };

        template <class... Args>
        using Transit = TransitImpl<Args...>::Result;
    }

    template <class... Args>
    class S2f
        : public cmt::Future<s2f::Transit<Args...>>
    {
    public:
        S2f(sbs::Signal<void, Args...> s);
        ~S2f();

    private:
        sbs::Owner                          _sol;
        cmt::Promise<s2f::Transit<Args...>> _promise;
    };
}

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Args>
    S2f<Args...>::S2f(sbs::Signal<void, Args...> s)
    {
        cmt::Future<s2f::Transit<Args...>>::operator=(_promise.future());

        s += _sol * [this]<class... ArgsInput>(ArgsInput&&...args) requires requires { {_promise.resolveValue(std::forward<ArgsInput>(args)...)}; }
        {
            _sol.flush();
            if(!_promise.resolved())
                _promise.resolveValue(std::forward<ArgsInput>(args)...);
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Args>
    S2f<Args...>::~S2f()
    {
        _sol.flush();
    }
}
