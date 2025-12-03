// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/himpl.hpp>
#include <dci/qml/implMetaInfo.hpp>
#include "api.hpp"
#include <memory>
#include <QtGlobal>

QT_BEGIN_NAMESPACE
class QObject;
class QApplication;
class QQmlApplicationEngine;
QT_END_NAMESPACE

namespace dci::qml
{
    class App;
    using AppPtr = std::shared_ptr<App>;

    class API_DCI_QML App
        : public himpl::FaceLayout<App, impl::App>
    {
        App() = delete;
        App(const App&) = delete;
        void operator=(const App&) = delete;

    public:
        static AppPtr instance();

    public:
        ~App();

        QApplication* qapp();
        QQmlApplicationEngine* qengine();

        QObject* loadScript(const QString& filePath);
    };
}
