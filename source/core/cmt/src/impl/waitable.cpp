// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "waitable.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waitable::Waitable(bool (* canAcquire)(const Waitable*), bool (* tryAcquire)(Waitable*))
        : _links()
        , _canAcquire(canAcquire)
        , _tryAcquire(tryAcquire)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waitable::~Waitable()
    {
        _links.each([this](WWLink* link)
        {
            dbgAssert(this == link->_waitable);
            link->_waiter->waitableDead(link);
        });

        dbgAssert(_links.empty());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::tryDestruction(Waitable *)
    {
        //empty is ok
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::wait()
    {
        if(tryAcquire())
        {
            return;
        }

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waitable::canAcquire() const
    {
        return _canAcquire(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waitable::tryAcquire()
    {
        return _tryAcquire(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::beginAcquire(WWLink* link)
    {
        dbgAssert(!Scheduler::instance().currentFiber() || !Scheduler::instance().currentFiber()->task()->stopRequested());

        dbgAssert(this == link->_waitable);
        dbgAssert(!_links.contains(link));

        _links.push(link);
        _linksAmount++;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::endAcquire(WWLink* link)
    {
        dbgAssert(this == link->_waitable);
        dbgAssert(_links.contains(link));

        _links.remove(link);
        _linksAmount--;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waitable::throwTaskStopIfNeed()
    {
        ctx::Fiber* currentFiber = Scheduler::instance().currentFiber();

        if(currentFiber)
        {
            dbgAssert(currentFiber->task());
            if(currentFiber->task()->stopRequested())
            {
                throw cmt::task::Stop{};
            }
        }
    }

}
