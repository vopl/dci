// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
