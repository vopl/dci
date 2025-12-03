// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class EventDispatcher;
}

namespace dci::qml::eventDispatcher
{
    class Sockets
    {
    public:
        Sockets(EventDispatcher* ed);
        ~Sockets();

    public:
        void registerSocketNotifier(QSocketNotifier* notifier);
        void unregisterSocketNotifier(QSocketNotifier* notifier);

    public:
        bool sendEvents();

    public:
        void startingUp();
        void closingDown();

    private:
        void ready(QSocketNotifier* qsn);

    private:
        EventDispatcher* _ed{};
        struct State
        {
            Sockets *                           _ss;
            poll::Descriptor                    _descriptor;
            poll::descriptor::ReadyStateFlags   _readyState {};

            std::set<QSocketNotifier *> _notifiers;

            State(Sockets* ss, poll::descriptor::Native native);
            void callback(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState);
        };

        using States = std::map<poll::descriptor::Native, State>;
        States _states;

        std::set<QSocketNotifier*>   _readyNotifiers;
    };
}
