// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/sbs/wire/agg.hpp>
#include "noReply.hpp"

#include <dci/cmt/future.hpp>
#include <type_traits>

namespace dci::idl::contract
{
    template<typename T, sbs::wire::Agg agg = sbs::wire::Agg::first>
    using Res =
        std::conditional_t<
            std::is_same_v<NoReply, T>,
            void,
            std::conditional_t<
                agg != sbs::wire::Agg::all,
                cmt::Future<T>,
                std::vector<cmt::Future<T>>
            >
        >;
}
