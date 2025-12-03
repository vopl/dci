// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../def.hpp"
#include "../api/meth.hpp"
#include <dci/sbs/owner.hpp>
#include <QMetaType>

namespace dci::qml::qmeta
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <>
    struct Def<sbs::Owner>
    {
        using T = sbs::Owner;
        static constexpr bool _declared = true;
        static constexpr Name _name {utils::tname<T>};

        using Api = TList
        <
            api::Meth<"flush", [](T& o) { return o.flush(); }>
        >;
    };
}
