// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

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
                    QApplication::sendEvent(snp, &qe);
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
