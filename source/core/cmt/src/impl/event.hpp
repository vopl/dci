// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "waitable.hpp"
#include "raisable.hpp"

namespace dci::cmt::impl
{
    class Event final
        : public Waitable
        , public Raisable
    {
        Event(const Event&) = delete;
        void operator=(const Event&) = delete;

    public:
        Event();
        ~Event();
        static void tryDestruction(Event* e);

        bool isRaised() const;
        void reset();

    public:
        void wait();

    public:
        void raise();

    private:
        bool canAcquire() const;
        bool tryAcquire();

    private:
        bool _raised = false;
    };
}
