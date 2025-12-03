// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class Victim : std::set<QString>
    {
        Q_GADGET

        QML_SEQUENTIAL_CONTAINER(QString)
        QML_FOREIGN(std::set<QString>)
    };
}
