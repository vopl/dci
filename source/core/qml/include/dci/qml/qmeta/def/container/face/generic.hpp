// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "../../../common.hpp"
#include "../../../api/prop.hpp"

#include <QJSValue>

namespace dci::qml::qmeta::def::container::face
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class T>
    struct Generic
    {
        template <class Assembly>
        using Api = TList
        <
//            Array.prototype.length
            api::PropR<"length", [](const T& o) { return o.size(); }>
        >;
    };
}
