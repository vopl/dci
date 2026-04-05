/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "eventDispatcher.hpp"

#if __has_include(<sys/eventfd.h>)
#   include <sys/eventfd.h>
#endif

#include <qpa/qwindowsysteminterface.h>

#ifdef __unix__
#   include <QtGui/qpa/qplatformintegration.h>
#   include <QtWidgets/private/qapplication_p.h>
QT_BEGIN_NAMESPACE

    //хак для активации процессора событий XCB
    class QXcbConnection//Stub
    {
    public:
        void DCI_INTEGRATION_APIDECL_IMPORT processXcbEvents(QEventLoop::ProcessEventsFlags flags);
    };

    struct Stub1 {virtual ~Stub1();};
    struct Stub2 {virtual ~Stub2();};

    struct QXcbIntegrationStub : public QPlatformIntegration
    #ifndef QT_NO_OPENGL
        , public Stub1
    # if QT_CONFIG(egl)
        , public Stub2
    # endif
    #endif
    {
        QXcbConnection *connection() const { return m_connection; }

        QXcbConnection * m_connection;
    };
QT_END_NAMESPACE
#endif

namespace dci::qml
{

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    EventDispatcher::EventDispatcher()
        : _sockets{this}
        , _timers{this}
    {
        _awaker.woken() += _awakeOwner * [this]
        {
            _readyNotifier.raise();
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    EventDispatcher::~EventDispatcher()
    {
        _awakeOwner.flush();
        _sockets.closingDown();
        _timers.closingDown();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::socketsReady()
    {
        _readyNotifier.raise();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::timersReady()
    {
        _readyNotifier.raise();
    }

#ifdef __unix__
    namespace
    {
        //хак для активации процессора событий XCB
        void processXcb(QEventLoop::ProcessEventsFlags flags)
        {
            QPlatformIntegration* qtpi = QApplicationPrivate::platformIntegration();
            if(qtpi)
            {
                static_cast<QXcbIntegrationStub*>(qtpi)->connection()->processXcbEvents(flags);
            }
        }
    }
#endif

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool EventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
    {
#ifdef __unix__
        //хак для активации процессора событий XCB
        static bool isXcb = "xcb" == QApplication::platformName();
#endif

        bool res = false;

        dbgAssert(QThread::currentThread() == qApp->thread());

        _interrupt.store(false);
        const bool includeTimers    = !(flags & QEventLoop::X11ExcludeTimers);
        const bool includeNotifiers = !(flags & QEventLoop::ExcludeSocketNotifiers);
        const bool canWait          =  (flags & QEventLoop::WaitForMoreEvents);

        while(!_interrupt.load())
        {
            {
                bool localRes = true;

                while(localRes)
                {
                    localRes = false;

                    QCoreApplication::sendPostedEvents();

                    if(includeTimers)
                    {
                        localRes |= _timers.sendEvents();
                    }

                    if(includeNotifiers)
                    {
                        localRes |= _sockets.sendEvents();
                    }

#ifdef __unix__
                    //хак для активации процессора событий XCB
                    if(isXcb)
                    {
                        processXcb(flags);
                    }
#endif

                    localRes |= QWindowSystemInterface::sendWindowSystemEvents(flags);

                    res |= localRes;
                }
            }

            if(canWait)
            {
                try
                {
                    emit aboutToBlock();
                    _readyNotifier.wait();
                    emit awake();
                }
                catch(const cmt::task::Stop&)
                {
                    _interrupt.store(true);
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::registerSocketNotifier(QSocketNotifier* notifier)
    {
        dbgAssert(QThread::currentThread() == notifier->thread());
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _sockets.registerSocketNotifier(notifier);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::unregisterSocketNotifier(QSocketNotifier* notifier)
    {
        dbgAssert(QThread::currentThread() == notifier->thread());
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _sockets.unregisterSocketNotifier(notifier);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::registerTimer(int timerId, qint64 interval, Qt::TimerType timerType, QObject* object)
    {
        dbgAssert(QThread::currentThread() == object->thread());
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _timers.registerTimer(timerId, interval, timerType, object);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool EventDispatcher::unregisterTimer(int timerId)
    {
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _timers.unregisterTimer(timerId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool EventDispatcher::unregisterTimers(QObject* object)
    {
        dbgAssert(QThread::currentThread() == object->thread());
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _timers.unregisterTimers(object);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    QList<EventDispatcher::TimerInfo> EventDispatcher::registeredTimers(QObject* object) const
    {
        dbgAssert(QThread::currentThread() == object->thread());
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _timers.registeredTimers(object);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    int EventDispatcher::remainingTime(int timerId)
    {
        dbgAssert(QThread::currentThread() == qApp->thread());

        return _timers.remainingTime(timerId);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::wakeUp()
    {
        _awaker.wakeup();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::interrupt()
    {
        _interrupt.store(true);
        wakeUp();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::startingUp()
    {
        dbgAssert(QThread::currentThread() == qApp->thread());

        _sockets.startingUp();
        _timers.startingUp();

        QAbstractEventDispatcher::startingUp();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void EventDispatcher::closingDown()
    {
        dbgAssert(QThread::currentThread() == qApp->thread());

        _sockets.closingDown();
        _timers.closingDown();

        QAbstractEventDispatcher::closingDown();
    }
}
