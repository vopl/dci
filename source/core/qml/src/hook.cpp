// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "hook.hpp"
#include <dci/qml/app.hpp>
#include "ep/manager.hpp"

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hook::Hook(QObject* parent)
        : QObject{parent}
    {
        qRegisterMetaType<QAbstractItemModel*>("QAbstractItemModel*");
        qRegisterMetaType<QObjectList>("QObjectList");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Hook::~Hook()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QAbstractItemModel* Hook::allAsModel(QString tag)
    {
        return ep::g_manager.select(tag)->asModel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObjectList Hook::all(QString tag)
    {
        return ep::g_manager.select(tag)->asArray();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QAbstractItemModel* Hook::oneAsModel(QString tag)
    {
        return ep::g_manager.select(tag, 1)->asModel();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* Hook::one(QString tag)
    {
        return ep::g_manager.select(tag, 1)->asObject();
    }
}
