// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include <dci/poll/timer.hpp>
#include "impl/timer.hpp"

namespace dci::poll
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval)
        : himpl::FaceLayout<Timer, impl::Timer>{interval, false, nullptr, nullptr}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, bool repeatable)
        : himpl::FaceLayout<Timer, impl::Timer>{interval, repeatable, nullptr, nullptr}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, cmt::task::Owner* tickOwner)
        : himpl::FaceLayout<Timer, impl::Timer>{interval, false, tickOwner, nullptr}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, bool repeatable, cmt::task::Owner* tickOwner)
        : himpl::FaceLayout<Timer, impl::Timer>{interval, repeatable, tickOwner, nullptr}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, cmt::Raisable* raisable)
        : himpl::FaceLayout<Timer, impl::Timer>{interval, false, nullptr, raisable}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, bool repeatable, cmt::Raisable* raisable)
        : himpl::FaceLayout<Timer, impl::Timer>{interval, repeatable, nullptr, raisable}
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::~Timer()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    sbs::Signal<> Timer::tick()
    {
        return impl().tick();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::setTickOwner(cmt::task::Owner* tickOwner)
    {
        return impl().setTickOwner(tickOwner);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::resetTickOwner()
    {
        return impl().resetTickOwner();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::setRaisable(cmt::Raisable* raisable)
    {
        return impl().setRaisable(raisable);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::resetRaisable()
    {
        return impl().resetRaisable();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::chrono::nanoseconds Timer::remaining() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(impl().remaining());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::chrono::nanoseconds Timer::interval() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(impl().interval());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::interval(std::chrono::nanoseconds v)
    {
        return impl().interval(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timer::repeatable() const
    {
        return impl().repeatable();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::repeatable(bool v)
    {
        return impl().repeatable(v);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::start()
    {
        return impl().start();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::restart()
    {
        return impl().restart();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool Timer::started() const
    {
        return impl().started();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Timer::stop()
    {
        return impl().stop();
    }
}
