// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"
#include <memory>
#include <type_traits>
#include <QtGlobal>

QT_BEGIN_NAMESPACE
    class QObject;
    class QString;
QT_END_NAMESPACE

namespace dci::qml
{
    class App;
}

namespace dci::qml::ep
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    struct API_DCI_QML Extension
    {
        virtual ~Extension();
        virtual void startup(App*);
        virtual void shutdown(App*);
        virtual QObject* activate(App*, const QString& tag) = 0;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Startup, class Shutdown, class Activate>
    requires
    (
        (std::is_invocable_r_v<void, std::decay_t<Startup>, App*> || std::is_invocable_r_v<void, std::decay_t<Startup>>) &&
        (std::is_invocable_r_v<void, std::decay_t<Shutdown>, App*> || std::is_invocable_r_v<void, std::decay_t<Shutdown>>) &&
        (std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*, const QString&> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>>)
    )
    std::unique_ptr<Extension> uniqueExtension(Startup&& startup, Shutdown&& shutdown, Activate&& activate, const QString& tag, int priority = 0);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Activate>
    requires
    (
        (std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*, const QString&> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>>)
    )
    std::unique_ptr<Extension> uniqueExtension(Activate&& activate, const QString& tag, int priority = 0);

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void API_DCI_QML registrate(Extension* e, const QString& tag, int priority = 0);
    void API_DCI_QML deregistrate(Extension* e);
    void API_DCI_QML deregistrate(Extension* e, const QString& tag);
}

namespace dci::qml::ep
{
    template <class Startup, class Shutdown, class Activate>
    requires
    (
        (std::is_invocable_r_v<void, std::decay_t<Startup>, App*> || std::is_invocable_r_v<void, std::decay_t<Startup>>) &&
        (std::is_invocable_r_v<void, std::decay_t<Shutdown>, App*> || std::is_invocable_r_v<void, std::decay_t<Shutdown>>) &&
        (std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*, const QString&> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>>)
    )
    std::unique_ptr<Extension> uniqueExtension(Startup&& startup, Shutdown&& shutdown, Activate&& activate, const QString& tag, int priority)
    {
        struct Instance : Extension
        {
            Instance(Startup&& startup, Shutdown&& shutdown, Activate&& activate, const QString& tag, int priority)
                : _startup{std::forward<Startup>(startup)}
                , _shutdown{std::forward<Shutdown>(shutdown)}
                , _activate{std::forward<Activate>(activate)}
            {
                registrate(this, tag, priority);
            }

            ~Instance() override
            {
                deregistrate(this);
            }

        private:
            void startup(App* app) override
            {
                if constexpr(std::is_invocable_r_v<void, std::decay_t<Startup>, App*>)
                    return _startup(app);
                else
                    return _startup();
            }

            void shutdown(App* app) override
            {
                if constexpr(std::is_invocable_r_v<void, std::decay_t<Shutdown>, App*>)
                    return _shutdown(app);
                else
                    return _shutdown();
            }

            QObject* activate(App* app, const QString& tag) override
            {
                if constexpr(std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*, const QString&>)
                    return _activate(app, tag);
                else if constexpr(std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*>)
                    return _activate(app);
                else
                    return _activate();
            }

        private:
            std::decay_t<Startup> _startup;
            std::decay_t<Shutdown> _shutdown;
            std::decay_t<Activate> _activate;
        };

        return std::make_unique<Instance>(
                    std::forward<Startup>(startup),
                    std::forward<Shutdown>(shutdown),
                    std::forward<Activate>(activate),
                    tag, priority);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Activate>
    requires
    (
        (std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*, const QString&> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>, App*> || std::is_invocable_r_v<QObject*, std::decay_t<Activate>>)
    )
    std::unique_ptr<Extension> uniqueExtension(Activate&& activate, const QString& tag, int priority)
    {
        return uniqueExtension([]{}, []{}, std::forward<Activate>(activate), tag, priority);
    }
}
