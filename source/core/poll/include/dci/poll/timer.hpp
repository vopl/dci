// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/poll/api.hpp>
#include <dci/himpl.hpp>
#include <dci/poll/implMetaInfo.hpp>
#include <dci/cmt/task/owner.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/sbs/signal.hpp>

#include <chrono>
#include <concepts>

namespace dci::poll
{
    class API_DCI_POLL Timer
        : public himpl::FaceLayout<Timer, impl::Timer>
    {
        Timer(const Timer&) = delete;
        void operator=(const Timer&) = delete;

    public:
        Timer(std::chrono::nanoseconds interval = std::chrono::seconds{1});
        Timer(std::chrono::nanoseconds interval, bool repeatable);

        Timer(std::chrono::nanoseconds interval, auto&& onTick, cmt::task::Owner* tickOwner = nullptr) requires(std::invocable<decltype(onTick)&&>);
        Timer(std::chrono::nanoseconds interval, bool repeatable, auto&& onTick, cmt::task::Owner* tickOwner = nullptr) requires(std::invocable<decltype(onTick)&&>);

        Timer(std::chrono::nanoseconds interval, cmt::task::Owner* tickOwner);
        Timer(std::chrono::nanoseconds interval, bool repeatable, cmt::task::Owner* tickOwner);

        Timer(std::chrono::nanoseconds interval, cmt::Raisable* raisable);
        Timer(std::chrono::nanoseconds interval, bool repeatable, cmt::Raisable* raisable);

        ~Timer();

        sbs::Signal<> tick();

        void setTickOwner(cmt::task::Owner* owner);
        void resetTickOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        std::chrono::nanoseconds remaining() const;

        std::chrono::nanoseconds interval() const;
        void interval(std::chrono::nanoseconds v);

        bool repeatable() const;
        void repeatable(bool v);

        void start();
        void restart();
        bool started() const;
        void stop();
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, auto&& onTick, cmt::task::Owner* tickOwner) requires(std::invocable<decltype(onTick)&&>)
        : Timer{interval, tickOwner}
    {
        this->tick() += std::forward<decltype(onTick)>(onTick);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, bool repeatable, auto&& onTick, cmt::task::Owner* tickOwner) requires(std::invocable<decltype(onTick)&&>)
        : Timer{interval, repeatable, tickOwner}
    {
        this->tick() += std::forward<decltype(onTick)>(onTick);
    }
}
