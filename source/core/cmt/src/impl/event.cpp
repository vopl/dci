// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "event.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Event::Event()
        : Waitable(
            [](const Waitable* w){ return static_cast<const Event*>(w)->canAcquire(); },
            [](Waitable* w){ return static_cast<Event*>(w)->tryAcquire(); })
        , Raisable([](Raisable* r){ return static_cast<Event*>(r)->raise(); })
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Event::~Event()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Event::tryDestruction(Event* e)
    {
        e->~Event();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Event::isRaised() const
    {
        return _raised;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Event::reset()
    {
        _raised = false;

        _links.each([this](WWLink* link)
        {
            if(canAcquire())
            {
                return false;
            }
            link->_waiter->readyOffer<false>(link);
            return true;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Event::wait()
    {
        throwTaskStopIfNeed();

        if(_raised)
        {
            return;
        }

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Event::raise()
    {
        if(!_raised)
        {
            _raised = true;

            _links.each([this](WWLink* link)
            {
                link->_waiter->readyOffer(link);
                return _raised;
            });
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Event::canAcquire() const
    {
        return _raised;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Event::tryAcquire()
    {
        throwTaskStopIfNeed();

        return _raised;
    }
}
