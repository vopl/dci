// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "def.hpp"
#include <QMetaObject>

namespace dci::qml::qmeta
{
    template <class T>
    requires (Def<T>::_declared)
    struct Superdata
    {
        static constexpr QMetaObject::SuperData qt()
        {
            return {};
        }
    };
}
