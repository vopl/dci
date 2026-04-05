/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "timer.hpp"
#include "clocking/bucket.hpp"
#include "clocking/config.hpp"

#include <dci/utils/bits.hpp>
#include <chrono>

namespace dci::poll::impl
{
    class Clocking
    {

    public:
        using Duration      = clocking::Duration;
        using DurationRep   = clocking::DurationRep;
        using Clock         = clocking::Clock;
        using Point         = clocking::Point;
        using PointRep      = clocking::PointRep;

    public:
        Clocking();
        ~Clocking();

    public:
        static PointRep exactNow();
        PointRep now() const;

        void start(Timer* t);
        void update(Timer* t);
        void stop(Timer* t);

    public:
        bool hasPayload() const;
        bool fireTicks();

        Duration distance2NextPoint() const;

    private:
        PointRep rollBucketsTo(PointRep time);

    private:
        std::size_t bucketIndexFor(PointRep target);
        PointRep calculateBucketStart(std::size_t index);
        clocking::Bucket* getOrMakeBucket(std::size_t index);

    private:
        PointRep _now;
        PointRep _nearestPoint;

        std::size_t _amount = 0;

        static constexpr std::size_t _bucketsAmount = dci::utils::bits::bitsof<PointRep>()-(std::is_signed_v<PointRep> ? 1 : 0);
        clocking::BucketPtr _buckets[_bucketsAmount] = {};

        std::size_t _readyAmount = 0;
        clocking::Bucket _readyBucket;
    };
}
