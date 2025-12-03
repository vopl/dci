// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <cstdint>
#include "body.hpp"
#include <dci/utils/intrusiveDlist.hpp>

namespace dci::cmt::impl::task
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    class Owner final
    {
        Owner(const Owner&) = delete;
        Owner(Owner&&) = delete;
        void operator=(const Owner&) = delete;
        void operator=(Owner&&) = delete;

    public:
        Owner();
        ~Owner();

    public:
        void subscribe(Body* task);
        void unsubscribe(Body* task);

        bool stopRequested() const;
        bool empty() const;

        void flush(bool andWait);
        void stop(bool andWait);
        void wait();

    private:
        void stopImpl(bool once, bool andWait);

    private:
        utils::IntrusiveDlist<Body, Owner>  _tasks;
        utils::IntrusiveDlist<Body, Owner>  _waiting;
        bool                                _waitingActive{};
        bool                                _stopRequested{false};
    };
}
