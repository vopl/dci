// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "waiter.hpp"
#include "../waitable.hpp"
#include "../scheduler.hpp"
#include <dci/cmt/task/stop.hpp>
#include <dci/utils/atScopeExit.hpp>
#include <climits>

namespace dci::cmt::impl::details
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::Waiter(WWLink* links, std::size_t amount)
        : _links(links)
        , _linksAmount(amount)
        , _mode{ModeSync{}}
        , _state{StateNull{}}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::Waiter(WWLink* links, std::size_t amount, void(*cb)(void* cbData), void* cbData)
        : _links(links)
        , _linksAmount(amount)
        , _mode{ModeAsync{cb, cbData}}
        , _state{StateNull{}}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Waiter::~Waiter()
    {
        reset();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::any(std::size_t* acquiredIndex)
    {
        exec<StateAny>(acquiredIndex);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::all()
    {
        exec<StateAll>();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::expr(ExprEvaluator ee, void* eeData, std::byte* bitsInBytes)
    {
        exec<StateExpr>(ee, eeData, bitsInBytes);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::reset()
    {
        endAcquire();
        _state = StateNull{};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ctx::Fiber* Waiter::fiber()
    {
        return _mode.visit([]<class Mode>(Mode& mode) -> ctx::Fiber*
        {
            if constexpr(std::is_same_v<ModeSync, Mode>)
            {
                return mode._fiber;
            }

            if constexpr(std::is_same_v<ModeAsync, Mode>)
            {
                return Scheduler::instance().currentFiber();
            }

            dbgFatal("never here");
            return {};
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waiter::readyOffer(WWLink* link)
    {
        dbgAssert(link >= _links && link < _links+_linksAmount);
        dbgAssert(this == link->_waiter);

        return _state.visit([&]<class State>(State& state)
        {
            if(!ready(state, link))
            {
                return false;
            }
            commit(state, link);
            reset();

            _mode.visit([&]<class Mode>(Mode& mode)
            {
                if constexpr(std::is_same_v<ModeSync, Mode>)
                {
                    Scheduler::instance().ready(mode._fiber);
                    return;
                }

                if constexpr(std::is_same_v<ModeAsync, Mode>)
                {
                    mode._cb(mode._cbData);
                    return;
                }

                dbgFatal("never here");
            });

            return true;
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::waitableDead(WWLink* link)
    {
        dbgAssert(link >= _links && link < _links+_linksAmount);
        dbgAssert(this == link->_waiter);

        if(WWLink::State::regularConnected == link->_state)
        {
            dbgAssert(link->_waitable);
            dbgAssert(link->_waiter);
            link->_waitable->endAcquire(link);
            link->_state = WWLink::State::regular;
        }

        link->_waitable = nullptr;
        readyOffer(link);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waiter::StateExpr::getBit(std::size_t index) const
    {
        std::size_t byte = index / CHAR_BIT;
        std::size_t bitInByte = index % CHAR_BIT;

        return std::byte{1} == ((_bitsInBytes[byte] >> bitInByte) & std::byte{1});
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::StateExpr::setBit(std::size_t index)
    {
        std::size_t byte = index / CHAR_BIT;
        std::size_t bitInByte = index % CHAR_BIT;

        _bitsInBytes[byte] |= (std::byte{1} << bitInByte);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::StateExpr::resetBit(std::size_t index)
    {
        std::size_t byte = index / CHAR_BIT;
        std::size_t bitInByte = index % CHAR_BIT;

        _bitsInBytes[byte] &= ~(std::byte{1} << bitInByte);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class State> void Waiter::exec(auto&&... args)
    {
        dbgAssert(_state.holds<StateNull>());

        if(ctx::Fiber* currentFiber = Scheduler::instance().currentFiber())
        {
            dbgAssert(currentFiber->task());
            if(currentFiber->task()->stopRequested())
                throw cmt::task::Stop{};
        }

        State& state = _state.emplace<State>(std::forward<decltype(args)>(args)...);

        if(ready(state))
        {
            commit(state);
            reset();

            _mode.visit([&]<class Mode>(Mode& mode)
            {
                if constexpr(std::is_same_v<ModeSync, Mode>)
                {
                    return;
                }

                if constexpr(std::is_same_v<ModeAsync, Mode>)
                {
                    mode._cb(mode._cbData);
                    return;
                }

                dbgFatal("never here");
            });

            return;
        }

        beginAcquire();

        _mode.visit([&]<class Mode>(Mode& mode)
        {
            if constexpr (std::is_same_v<ModeSync, Mode>)
            {
                mode._fiber = Scheduler::instance().currentFiber();
                Scheduler::instance().hold();

                task::Body* task = mode._fiber->task();
                reset();
                if(task->stopRequested())
                    throw cmt::task::Stop{};

                return;
            }

            if constexpr (std::is_same_v<ModeAsync, Mode>)
            {
                return;
            }
        });
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waiter::ready(StateNull& /*state*/, WWLink* /*offeredFrom*/)
    {
        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waiter::ready(StateAll& /*state*/, WWLink* offeredFrom)
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(&link != offeredFrom && link._waitable && !link._waitable->canAcquire())
            {
                return false;
            }
        }

        return true;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waiter::ready(StateAny& state, WWLink* offeredFrom)
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(&link == offeredFrom || !link._waitable || link._waitable->canAcquire())
            {
                *state._acquiredIndex = linkIndex;
                return true;
            }
        }

        return false;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Waiter::ready(StateExpr& state, WWLink* offeredFrom)
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(&link == offeredFrom || !link._waitable || link._waitable->canAcquire())
            {
                state.setBit(linkIndex);
            }
            else
            {
                state.resetBit(linkIndex);
            }
        }

        return state._evaluator(state._evaluatorData);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::commit(StateNull& /*state*/, WWLink* /*offeredFrom*/)
    {
        dbgFatal("never here");
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::commit(StateAll& /*state*/, WWLink* offeredFrom)
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(WWLink::State::repeat == link._state)
            {
                continue;
            }
            if(&link != offeredFrom && link._waitable)
            {
                bool b = link._waitable->tryAcquire();
                dbgAssert(b);
                (void)b;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::commit(StateAny& state, WWLink* offeredFrom)
    {
        dbgAssert(*state._acquiredIndex < _linksAmount);
        WWLink& link = _links[*state._acquiredIndex];
        dbgAssert(WWLink::State::repeat != link._state);
        if(&link != offeredFrom && link._waitable)
        {
            dbgAssert(link._waitable->canAcquire());
            bool b = link._waitable->tryAcquire();
            dbgAssert(b);
            (void)b;
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::commit(StateExpr& state, WWLink* offeredFrom)
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(WWLink::State::repeat == link._state)
            {
                continue;
            }
            if(state.getBit(linkIndex) && &link != offeredFrom && link._waitable)
            {
                dbgAssert(link._waitable->canAcquire());
                bool b = link._waitable->tryAcquire();
                dbgAssert(b);
                (void)b;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::beginAcquire()
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(WWLink::State::regular == link._state && link._waitable)
            {
                dbgAssert(!link._waiter);
                link._waiter = this;
                link._waitable->beginAcquire(&link);
                link._state = WWLink::State::regularConnected;
            }
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Waiter::endAcquire()
    {
        for(std::size_t linkIndex{}; linkIndex<_linksAmount; ++linkIndex)
        {
            WWLink& link = _links[linkIndex];
            if(WWLink::State::regularConnected == link._state)
            {
                dbgAssert(link._waitable);
                dbgAssert(this == link._waiter);
                link._waitable->endAcquire(&link);
                link._state = WWLink::State::regular;
            }
        }
    }
}
