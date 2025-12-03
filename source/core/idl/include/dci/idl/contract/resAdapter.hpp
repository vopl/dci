// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/sbs/wire/transfer/ret.hpp>
#include <dci/cmt/future.hpp>
#include "../interface/exception/methodNotConnected.hpp"

namespace dci::idl::contract
{
    struct ResAdapter
    {
        template <class R, dci::sbs::wire::Agg agg>
        R operator()(dci::sbs::wire::transfer::Ret<R, agg>& ret)
        {
            return ret.detach();
        }

        template <class T, dci::sbs::wire::Agg agg>
        cmt::Future<T> operator()(dci::sbs::wire::transfer::Ret<cmt::Future<T>, agg>& ret)
        {
            if(ret._assigned)
            {
                return std::move(ret._value);
            }

            return cmt::readyFuture<T>(std::make_exception_ptr(interface::exception::MethodNotConnected()));
        }

        template <class T>
        std::vector<cmt::Future<T>> operator()(dci::sbs::wire::transfer::Ret<cmt::Future<T>, dci::sbs::wire::Agg::all>& ret)
        {
            return std::move(ret._value);
        }
    };
}
