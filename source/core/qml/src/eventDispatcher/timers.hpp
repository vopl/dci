// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::qml
{
    class EventDispatcher;
}

namespace dci::qml::eventDispatcher
{
    class Timers
    {
        using TimerInfo = QAbstractEventDispatcher::TimerInfo;

    public:
        Timers(EventDispatcher* ed);
        ~Timers();

    public:
        void registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject* object);
        bool unregisterTimer(int timerId);
        bool unregisterTimers(QObject* object);
        QList<TimerInfo> registeredTimers(QObject* object) const;

        int remainingTime(int timerId);

    public:
        bool sendEvents();

    public:
        void startingUp();
        void closingDown();

    private:
        struct State;
        void ready(State* state);

    private:
        EventDispatcher* _ed;

        struct State
        {
            Timers *        _ts;
            poll::Timer     _timer;
            QObject *       _receiver {};
            TimerInfo       _info;

            State(Timers* ts, QObject* receiver, int id, int interval, Qt::TimerType type);
        };
        using States = std::map<int, State>;
        using StatesByReceiver = std::multimap<QObject*, State*>;

        States              _states;
        StatesByReceiver    _statesByReceiver;

        std::set<State*>    _readyStates;
    };
}
