// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include <dci/qml/app.hpp>
#include "impl/app.hpp"

namespace dci::qml
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    AppPtr App::instance()
    {
        return impl::App::instance();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    App::~App()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QApplication* App::qapp()
    {
        return impl().qapp();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QQmlApplicationEngine* App::qengine()
    {
        return impl().qengine();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QObject* App::App::loadScript(const QString& filePath)
    {
        return impl().loadScript(filePath);
    }
}
