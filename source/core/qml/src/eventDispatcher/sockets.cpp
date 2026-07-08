/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#include "pch.hpp"
#include "sockets.hpp"
#include "../eventDispatcher.hpp"

namespace dci::qml::eventDispatcher
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sockets::Sockets(EventDispatcher* ed)
        : _ed{ed}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sockets::~Sockets()
    {
        closingDown();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::registerSocketNotifier(QSocketNotifier* notifier)
    {
        poll::descriptor::Native native{notifier->socket()};
        State& s = _states.try_emplace(
                       native,
                       this, native).first->second;

        s._notifiers.insert(notifier);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::unregisterSocketNotifier(QSocketNotifier* notifier)
    {
        poll::descriptor::Native native{notifier->socket()};

        auto iter = _states.find(native);
        if(_states.end() == iter)
        {
            return;
        }

        State& s = iter->second;
        s._notifiers.erase(notifier);
        if(s._notifiers.empty())
        {
            s._descriptor.detach();
            _states.erase(iter);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Sockets::sendEvents()
    {
        bool res = false;

        {
            QEvent qe{QEvent::SockAct};

            auto iter{_readyNotifiers.begin()};
            while(iter!=_readyNotifiers.end())
            {
                QSocketNotifier* snp = *iter;
                iter = _readyNotifiers.erase(iter);

                if(snp->isEnabled())
                {
                    QCoreApplication::sendEvent(snp, &qe);
                    res = true;
                }
            }
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::startingUp()
    {
        //empty
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::closingDown()
    {
        for(auto &[fd, s] : _states)
        {
            s._descriptor.detach();
            s._notifiers.clear();
        }

        _states.clear();
        _readyNotifiers.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::ready(QSocketNotifier* qsn)
    {
        _readyNotifiers.emplace(qsn);
        _ed->socketsReady();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Sockets::State::State(Sockets* ss, poll::descriptor::Native native)
        : _ss{ss}
        , _descriptor{native, [this](poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState){callback(native, readyState);}}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Sockets::State::callback(poll::descriptor::Native native, poll::descriptor::ReadyStateFlags readyState)
    {
        (void)native;

        dbgAssert(_descriptor.native() == native);

        _readyState |= readyState;

        std::uint_fast32_t errMask = poll::descriptor::rsf_error | poll::descriptor::rsf_close | poll::descriptor::rsf_eof;

        for(QSocketNotifier* qsn : _notifiers)
        {
            switch(qsn->type())
            {
            case QSocketNotifier::Read:
                if(_readyState & (errMask | poll::descriptor::rsf_read))
                {
                    _readyState &= ~poll::descriptor::rsf_read;
                    _ss->ready(qsn);
                }
                break;

            case QSocketNotifier::Write:
                if(_readyState & (errMask | poll::descriptor::rsf_write))
                {
                    _readyState &= ~poll::descriptor::rsf_write;
                    _ss->ready(qsn);
                }
                break;

            case QSocketNotifier::Exception:
                if(_readyState & (errMask | poll::descriptor::rsf_pri))
                {
                    _readyState &= ~poll::descriptor::rsf_pri;
                    _ss->ready(qsn);
                }
                break;

            default:
                break;
            }
        }
    }
}
