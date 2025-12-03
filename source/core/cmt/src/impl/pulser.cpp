// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pulser.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pulser::Pulser(WakeMode wakeMode)
        : Waitable(
            [](const Waitable* w){ return static_cast<const Pulser*>(w)->canAcquire(); },
            [](Waitable* w){ return static_cast<Pulser*>(w)->tryAcquire(); })
        , Raisable([](Raisable* r){ return static_cast<Pulser*>(r)->raise(); })
        , _wakeMode(wakeMode)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Pulser::~Pulser()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pulser::tryDestruction(Pulser* p)
    {
        p->~Pulser();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pulser::wait()
    {
        throwTaskStopIfNeed();

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Pulser::raise()
    {
        _links.each([this](WWLink* link)
        {
            if(link->_waiter->readyOffer(link) && WakeMode::one == _wakeMode)
                return false;
            return true;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Pulser::canAcquire() const
    {
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Pulser::tryAcquire()
    {
        throwTaskStopIfNeed();

        return false;
    }
}
