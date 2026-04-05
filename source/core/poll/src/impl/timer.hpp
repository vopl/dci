/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "clocking/bucketElement.hpp"
#include "clocking/config.hpp"
#include <dci/cmt/task/owner.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/sbs/wire.hpp>
#include <chrono>
#include <memory>

namespace dci::poll::impl
{
    class Timer final
        : public clocking::BucketElement
    {
    public:
        using Duration      = clocking::Duration;
        using DurationRep   = clocking::DurationRep;
        using Clock         = clocking::Clock;
        using Point         = clocking::Point;
        using PointRep      = clocking::PointRep;

    private:
        Timer(const Timer&) = delete;
        void operator=(const Timer&) = delete;

    public:
        Timer(Duration interval,
                bool repeatable,
                cmt::task::Owner* tickOwner,
                cmt::Raisable* raisable);

        ~Timer();

        sbs::Signal<> tick();

        void setTickOwner(cmt::task::Owner* tickOwner);
        void resetTickOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        Duration remaining() const;

        Duration interval() const;
        void interval(Duration v);

        bool repeatable() const;
        void repeatable(bool v);

        void start();
        void restart();
        bool started() const;
        void stop();

    public:
        PointRep nextPoint() const;
        void tick(PointRep now);

    private:
        DurationRep         _interval {};
        bool                _repeatable{};

        bool                _started{};

        struct Tick
        {
            bool _inProgress{};
            sbs::Wire<> _wire;
        };
        using TickPtr = std::shared_ptr<Tick>;
        TickPtr _tick{std::make_shared<Tick>()};

        cmt::task::Owner*   _tickOwner{};
        cmt::task::Owner    _localTickOwner{};
        cmt::Raisable*      _raisable{};
    };
}
