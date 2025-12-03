// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../agg.hpp"
#include "../../exception/wireNotConnected.hpp"
#include <vector>

namespace dci::sbs::wire::transfer
{

    template <class R, Agg agg>
    struct Ret;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    struct Ret<void, Agg::first>
    {
        void detach() {}
    };

    template <>
    struct Ret<void, Agg::last>
    {
        void detach() {}
    };

    template <>
    struct Ret<void, Agg::all>
    {
        void detach() {}
    };

    namespace
    {
        template <class E, class R>
        void chargeException(const R&)
        {
            throw E{};
        }

        template <class E, class Future, class = typename Future::Promise::Future>
        void chargeException(Future& future)
        {
            typename Future::Promise promise;
            promise.resolveException(std::make_exception_ptr(E{}));
            future = promise.future();
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R>
    struct Ret<R, Agg::first>
    {
        R       _value;
        bool    _assigned = false;

        R detach()
        {
            if(!_assigned)
            {
                chargeException<exception::WireNotConnected>(_value);
            }

            return std::move(_value);
        }
    };

    template <class R>
    struct Ret<R, Agg::last>
    {
        R       _value;
        bool    _assigned = false;

        R detach()
        {
            if(!_assigned)
            {
                chargeException<exception::WireNotConnected>(_value);
            }

            return std::move(_value);
        }
    };

    template <class R>
    struct Ret<R, Agg::all>
    {
        std::vector<R>  _value;

        std::vector<R> detach()
        {
            return std::move(_value);
        }
    };
}
