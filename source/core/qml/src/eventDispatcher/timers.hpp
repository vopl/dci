/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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
