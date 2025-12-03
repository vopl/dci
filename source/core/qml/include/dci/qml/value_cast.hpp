// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <dci/utils/dbg.hpp>

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    struct ValueCastImpl;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class To, class From>
    To value_cast(const From& /*from*/)
    {
        dbgFatal("not impl");
    }
}
