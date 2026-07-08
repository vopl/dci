/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "timers.hpp"
#include "../eventDispatcher.hpp"

namespace dci::qml::eventDispatcher
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timers::Timers(EventDispatcher* ed)
        : _ed{ed}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timers::~Timers()
    {
        closingDown();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timers::registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject* object)
    {
        State& s = _states.try_emplace(
                       timerId,
                       this, object, timerId, interval, timerType).first->second;
        dbgAssert(s._receiver == object);

        _statesByReceiver.emplace(object, &s);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timers::unregisterTimer(int timerId)
    {
        auto iter = _states.find(timerId);
        if(_states.end() == iter)
        {
            return false;
        }

        State& s = iter->second;

        auto iter2 = _statesByReceiver.find(s._receiver);
        while(_statesByReceiver.end() != iter2 && iter2->second->_receiver == s._receiver)
        {
            if(iter2->second->_info.timerId == timerId)
            {
                iter2 = _statesByReceiver.erase(iter2);
            }
            else
            {
                ++iter2;
            }
        }

        _readyStates.erase(&s);
        _states.erase(iter);

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timers::unregisterTimers(QObject* object)
    {
        auto iter = _statesByReceiver.find(object);
        if(_statesByReceiver.end() == iter)
        {
            return false;
        }

        while(_statesByReceiver.end() != iter && iter->second->_receiver == object)
        {
            _readyStates.erase(iter->second);
            _states.erase(iter->second->_info.timerId);
            iter = _statesByReceiver.erase(iter);
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QList<Timers::TimerInfo> Timers::registeredTimers(QObject* object) const
    {
        QList<EventDispatcher::TimerInfo> res;

        auto iter = _statesByReceiver.find(object);
        while(_statesByReceiver.end() != iter && iter->second->_receiver == object)
        {
            res << iter->second->_info;
            ++iter;
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int Timers::remainingTime(int timerId)
    {
        auto iter = _states.find(timerId);
        if(_states.end() == iter)
        {
            return -1;
        }

        return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(iter->second._timer.remaining()).count());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timers::sendEvents()
    {
        bool res = !_readyStates.empty();

        while(!_readyStates.empty())
        {
            State* s = *_readyStates.begin();
            _readyStates.erase(_readyStates.begin());

            QTimerEvent e(s->_info.timerId);
            QGuiApplication::sendEvent(s->_receiver, &e);
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timers::startingUp()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timers::closingDown()
    {
        _states.clear();
        _statesByReceiver.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timers::ready(State* state)
    {
        _readyStates.insert(state);
        _ed->timersReady();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timers::State::State(Timers* ts, QObject* receiver, int id, int interval, Qt::TimerType type)
        : _ts{ts}
        , _timer{std::chrono::milliseconds{interval}, true, [this]{_ts->ready(this);}}
        , _receiver{receiver}
        , _info{id, interval, type}
    {
        _timer.start();
    }
}
