/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

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

        Timer(std::chrono::nanoseconds interval, auto&& onTick) requires(std::invocable<decltype(onTick)&&>);
        Timer(std::chrono::nanoseconds interval, bool repeatable, auto&& onTick) requires(std::invocable<decltype(onTick)&&>);

        Timer(std::chrono::nanoseconds interval, cmt::Raisable* raisable);
        Timer(std::chrono::nanoseconds interval, bool repeatable, cmt::Raisable* raisable);

        ~Timer();

        sbs::Signal<> tick();

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
    Timer::Timer(std::chrono::nanoseconds interval, auto&& onTick) requires(std::invocable<decltype(onTick)&&>)
        : Timer{interval}
    {
        this->tick() += std::forward<decltype(onTick)>(onTick);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Timer::Timer(std::chrono::nanoseconds interval, bool repeatable, auto&& onTick) requires(std::invocable<decltype(onTick)&&>)
        : Timer{interval, repeatable}
    {
        this->tick() += std::forward<decltype(onTick)>(onTick);
    }
}
