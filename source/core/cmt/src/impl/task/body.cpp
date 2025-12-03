// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "body.hpp"
#include "face.hpp"
#include "owner.hpp"
#include "../scheduler.hpp"
#include <dci/cmt/task/body.hpp>
#include <dci/cmt/task/stop.hpp>
#include <dci/himpl.hpp>

namespace dci::cmt::impl::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Body::Body(Owner* owner, cmt::task::CallAndDestroyExecutor callAndDestroyExecutor)
        : _owner(owner)
        , _callAndDestroyExecutor(callAndDestroyExecutor)
        , _stopRequested(_owner ? _owner->stopRequested() : false)
    {
        if(_owner)
        {
            _owner->subscribe(this);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Body::~Body()
    {
        dbgAssert(!(utils::IntrusiveDlistElement<Body, Scheduler>::emplaced()));

        _sbsOwner.flush();

        if(_owner)
        {
            _owner->unsubscribe(this);
            _owner = nullptr;
        }

        dbgAssert(!(utils::IntrusiveDlistElement<Body, Owner>::emplaced()));

        _faces.each([](Face* face)
        {
            face->detachBody();
        });

        dbgAssert(_faces.empty());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::setFiber(ctx::Fiber* fiber)
    {
        dbgAssert(fiber);
        _fiber = fiber;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ctx::Fiber* Body::fiber()
    {
        return _fiber;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::callAndDestroy() noexcept(true)
    {
        _callAndDestroyExecutor(himpl::impl2Face<cmt::task::Body>(this), !_stopRequested, true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::destroy() noexcept(true)
    {
        _callAndDestroyExecutor(himpl::impl2Face<cmt::task::Body>(this), false, true);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::subscribe(Face* face)
    {
        _faces.push(face);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::unsubscribe(Face* face)
    {
        dbgAssert(_faces.contains(face));
        _faces.remove(face);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::setState(cmt::task::State state)
    {
        _state = state;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::task::Key Body::key() const
    {
        return reinterpret_cast<cmt::task::Key>(this);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Body::isCurrent() const
    {
        return Scheduler::instance().currentTask() == this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    cmt::task::State Body::state() const
    {
        return _state;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::stop(bool throwSelf)
    {
        _stopRequested = true;
        if(_fiber)
        {
            if(_fiber == Scheduler::instance().currentFiber())
            {
                if(throwSelf)
                {
                    throw cmt::task::Stop{};
                }
            }
            else
            {
                Scheduler::instance().ready(_fiber);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Body::stopRequested() const
    {
        return _stopRequested;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::ownTo(Owner* owner)
    {
        if(owner == _owner)
        {
            return;
        }

        if(_owner)
        {
            _owner->unsubscribe(this);
        }

        _owner = owner;

        if(_owner)
        {
            _owner->subscribe(this);
            _stopRequested = _owner->stopRequested();
        }
        else
        {
            _stopRequested = false;
        }

        if(_stopRequested)
        {
            if(_fiber == Scheduler::instance().currentFiber())
            {
                throw cmt::task::Stop{};
            }
            else
            {
                Scheduler::instance().ready(_fiber);
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Owner* Body::owner()
    {
        return _owner;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Body::detachOwner()
    {
        if(_owner)
        {
            _owner->unsubscribe(this);
            _owner = nullptr;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Owner* Body::sbsOwner()
    {
        return &_sbsOwner;
    }

}
