// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "root.hpp"
#include "../task/body.hpp"
#include <dci/mm/stack.hpp>

namespace dci::cmt::impl
{
    class Scheduler;
}

namespace dci::cmt::impl::ctx
{
    namespace fiber
    {
        template <bool needStack>
        class StackOrNone
        {
        protected:
            mm::Stack _stack;

            void assignStackIfNeed(mm::Stack&& stack)
            {
                _stack = std::move(stack);
            }

            void doStackCompactIfNeed()
            {
                _stack.compact();
            }
        };

        template <>
        class StackOrNone<false>
        {
        protected:
            void assignStackIfNeed(mm::Stack&&)
            {
            }

            void doStackCompactIfNeed()
            {
            }
        };
    }

    class Fiber
        : public utils::IntrusiveDlistElement<Fiber, Scheduler>
        , public Engine<Fiber>
        , private fiber::StackOrNone<Engine<Fiber>::_needStack>
    {
        Fiber& operator=(const Fiber&) = delete;

        Fiber(Scheduler* scheduler);
        ~Fiber();

    public:
        static Fiber* alloc(Scheduler* scheduler);
        void free();

        void setTask(task::Body* task);
        void resetTask();
        task::Body* task();

    public:
        template <class D2>
        void switchTo(Engine<D2>* to, bool doCompact = true)
        {
            if constexpr(Engine<Fiber>::_needStack)
            {
                if(doCompact)
                {
                    doStackCompactIfNeed();
                }
            }

            Engine::switchTo(to);
        }

    public:
        [[noreturn]] void contextProc();

    private:
        Scheduler*  _scheduler {};
        task::Body* _task {};

    private:
        friend class dci::cmt::impl::Scheduler;
    };
}
