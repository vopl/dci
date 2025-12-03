// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "../hook.hpp"

namespace dci::qml
{
    class App;
    using AppPtr = std::shared_ptr<App>;
}

namespace dci::qml::impl
{

    class App final
    {
    private:
        App(const App&) = delete;
        void operator=(const App&) = delete;

    public:
        static qml::AppPtr instance();

    public:
        App();
        ~App();

        QApplication* qapp();
        QQmlApplicationEngine* qengine();

        QObject* loadScript(const QString& filePath);

    private:
        static std::weak_ptr<qml::App> _instance;

    private:
        bool                    _qappStop {};
        QApplication            _qapp;
        cmt::task::Owner        _mainLoopOwner;
        QQmlApplicationEngine   _qengine;
        Hook                    _hook;
    };
}
