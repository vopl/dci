// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "transfer/arg.hpp"
#include "transfer/ret.hpp"
#include "agg.hpp"

namespace dci::sbs::wire
{
    struct TransferBase
    {
        Agg _agg;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class R, Agg agg, class... Args>
    struct Transfer
        : TransferBase
    {
        transfer::Ret<R, agg>               _ret;
        std::tuple<transfer::Arg<Args>...>  _args;

        template <class... RawArgs>
        Transfer(RawArgs&&... rawArgs)
            : TransferBase{agg}
            , _args(std::forward<RawArgs>(rawArgs)...)
        {
        }
    };
}
