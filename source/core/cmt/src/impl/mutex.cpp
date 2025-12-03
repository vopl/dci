// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "mutex.hpp"
#include "details/waiter.hpp"
#include "scheduler.hpp"
#include <dci/cmt/task/stop.hpp>

namespace dci::cmt::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mutex::Mutex(RecursionMode recursionMode)
        : Lockable(
              [](const Waitable* w){ return static_cast<const Mutex*>(w)->canLock(); },
              [](Waitable* w){ return static_cast<Mutex*>(w)->tryLock(); },
              [](Lockable* l){ return static_cast<Mutex*>(l)->unlock(); })
        , _recursionMode(recursionMode)
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Mutex::~Mutex()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::tryDestruction(Mutex* m)
    {
        m->~Mutex();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Mutex::canLock() const
    {
        return
                (RecursionMode::nonRecursive == _recursionMode && ( !_counter                                                 )) ||
                (RecursionMode::recursive    == _recursionMode && ( !_owner || Scheduler::instance().currentFiber() == _owner ));
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Mutex::tryLock()
    {
        throwTaskStopIfNeed();

        switch(_recursionMode)
        {
        case RecursionMode::nonRecursive:
            {
                if(!_counter)
                {
                    _counter = 1;
                    return true;
                }
            }
            break;

        case RecursionMode::recursive:
            {
                dbgAssert(Scheduler::instance().currentFiber());

                if(!_owner)
                {
                    dbgAssert(!_counter);
                    _owner = Scheduler::instance().currentFiber();
                    _counter++;
                    return true;
                }
                else if(Scheduler::instance().currentFiber() == _owner)
                {
                    _counter++;
                    return true;
                }
            }
            break;
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::lock()
    {
        if(tryLock())
        {
            return;
        }

        WWLink l;
        l._waitable = this;
        details::Waiter(&l, 1).all();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::unlock()
    {
        switch(_recursionMode)
        {
        case RecursionMode::nonRecursive:
            {
                if(!_counter)
                {
                    dbgAssert(_links.empty());
                    return;
                }

                _counter = 1;

                bool readyOfferAccepted{};
                _links.each([&,this](WWLink* link)
                {
                    if(link->_waiter->readyOffer(link))
                    {
                        dbgAssert(1 == _counter);
                        readyOfferAccepted = true;
                        return false;
                    }
                    return true;
                });

                if(readyOfferAccepted)
                    return;

                _counter = 0;
            }
            break;

        case RecursionMode::recursive:
            {
                dbgAssert(!!_owner == !!_counter);
                if(!_owner || !_counter)
                {
                    dbgAssert(_links.empty());
                    return;
                }

                _counter--;

                if(!_counter)
                {
                    _counter = 1;

                    bool readyOfferAccepted{};
                    _links.each([&,this](WWLink* link)
                    {
                        _owner = link->_waiter->fiber();
                        //_counter = 1;

                        if(link->_waiter->readyOffer(link))
                        {
                            //link may be destroyed already
                            //dbgAssert(link->_waiter->fiber() == _owner);
                            dbgAssert(1 == _counter);
                            readyOfferAccepted = true;
                            return;
                        }
                    });

                    if(readyOfferAccepted)
                        return;

                    _owner = nullptr;
                    _counter = 0;
                }
            }
            break;
        }

        _links.each([this](WWLink* link)
        {
            if(canLock())
            {
                return false;
            }
            link->_waiter->readyOffer<false>(link);
            return true;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Mutex::wait()
    {
        lock();
    }
}
