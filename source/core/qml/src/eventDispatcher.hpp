// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"
#include "eventDispatcher/sockets.hpp"
#include "eventDispatcher/timers.hpp"

namespace dci::qml
{
    class EventDispatcher
        : public QAbstractEventDispatcher
    {
        Q_OBJECT

    public:
        EventDispatcher();
        ~EventDispatcher() override;

    public:
        void socketsReady();
        void timersReady();

    private:
        bool processEvents(QEventLoop::ProcessEventsFlags flags) override;

    private:
        void registerSocketNotifier(QSocketNotifier* notifier) override;
        void unregisterSocketNotifier(QSocketNotifier* notifier) override;

    private:
        void registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject* object) override;
        bool unregisterTimer(int timerId) override;
        bool unregisterTimers(QObject* object) override;
        QList<TimerInfo> registeredTimers(QObject* object) const override;

        int remainingTime(int timerId) override;

    private:
        void wakeUp() override;
        void interrupt() override;

    private:
        void startingUp() override;
        void closingDown() override;

    private:
        std::atomic_bool    _interrupt{};
        cmt::Notifier       _readyNotifier;
        sbs::Owner          _awakeOwner;
        poll::Awaker        _awaker;

    private:
        eventDispatcher::Sockets    _sockets;
        eventDispatcher::Timers     _timers;
    };
}
