// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/utils/intrusiveDlist.hpp>
#include <dci/cmt/task/key.hpp>
#include <dci/cmt/task/state.hpp>
#include <dci/sbs/owner.hpp>

namespace dci::cmt::task
{
    class Body;
    using CallAndDestroyExecutor = void (*)(Body* task, bool call, bool destroy) noexcept(true);
}

namespace dci::cmt::impl::ctx
{
    class Fiber;
}

namespace dci::cmt::impl
{
    class Scheduler;
}

namespace dci::cmt::impl::task
{
    class Owner;
    class Face;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Body final
        : public utils::IntrusiveDlistElement<Body, Scheduler>
        , public utils::IntrusiveDlistElement<Body, Owner>

    {
    public:
        Body(Owner* owner, cmt::task::CallAndDestroyExecutor callAndDestroyExecutor);
        ~Body();

    public:
        void setFiber(ctx::Fiber* fiber);
        ctx::Fiber* fiber();
        void callAndDestroy() noexcept(true);
        void destroy() noexcept(true);

    public:
        void subscribe(Face* face);
        void unsubscribe(Face* face);

    public:
        void setState(cmt::task::State state);

    public:
        cmt::task::Key key() const;
        bool isCurrent() const;
        cmt::task::State state() const;

        void stop(bool throwSelf = true);
        bool stopRequested() const;

        void ownTo(Owner* owner);
        Owner* owner();
        void detachOwner();

        sbs::Owner* sbsOwner();
    private:
        ctx::Fiber* _fiber {nullptr};

        sbs::Owner _sbsOwner;

        Owner*  _owner;
        cmt::task::CallAndDestroyExecutor _callAndDestroyExecutor;

    private:
        cmt::task::State    _state {cmt::task::State::null};
        bool                _stopRequested {false};

    private:
        utils::IntrusiveDlist<Face, Body> _faces;
    };

}
