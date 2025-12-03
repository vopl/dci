// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class App;

    class Hook
        : public QObject
    {
        Q_OBJECT

    public:
        Hook(QObject* parent = nullptr);
        ~Hook() override;

        Q_INVOKABLE QAbstractItemModel* allAsModel(QString tag);
        Q_INVOKABLE QObjectList         all       (QString tag);
        Q_INVOKABLE QAbstractItemModel* oneAsModel(QString tag);
        Q_INVOKABLE QObject *           one       (QString tag);

    private:
        Q_DISABLE_COPY(Hook)
    };
}
