// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "notifier.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Notifier::Notifier(WakeMode wakeMode)
        : Waitable(
            [](const Waitable* w){ return static_cast<const Notifier*>(w)->canAcquire(); },
            [](Waitable* w){ return static_cast<Notifier*>(w)->tryAcquire(); })
        , Raisable([](Raisable* r){ return static_cast<Notifier*>(r)->raise(); })
        , _wakeMode(wakeMode)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Notifier::~Notifier()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::tryDestruction(Notifier* n)
    {
        n->~Notifier();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Notifier::isRaised() const
    {
        return _raised;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::reset()
    {
        _raised = false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::wait()
    {
        throwTaskStopIfNeed();

        if(_raised)
        {
            _raised = false;
            return;
        }

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Notifier::raise()
    {
        if(_raised)
        {
            return;
        }

        _raised = false;

        bool notifiedAtLeastOne = false;

        _links.each([&, this](WWLink* link)
        {
            if(link->_waiter->readyOffer(link))
            {
                notifiedAtLeastOne = true;
                if(WakeMode::one == _wakeMode)
                {
                    dbgAssert(false == _raised);
                    return false;
                }
            }
            return true;
        });

        _raised = !notifiedAtLeastOne;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Notifier::canAcquire() const
    {
        return _raised;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Notifier::tryAcquire()
    {
        throwTaskStopIfNeed();

        if(_raised)
        {
            _raised = false;
            return true;
        }

        return false;
    }
}
