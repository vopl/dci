/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/details/wwLink.hpp>

namespace dci::cmt::impl
{
    using namespace dci::cmt::details;

    class Waitable
    {
        Waitable(const Waitable&) = delete;
        void operator=(const Waitable&) = delete;

    public:
        Waitable(
                bool (* canAcquire)(const Waitable*),
                bool (* tryAcquire)(Waitable*));
        ~Waitable();
        static void tryDestruction(Waitable*);

        void wait();

    public:
        bool canAcquire() const;
        bool tryAcquire();

        void beginAcquire(WWLink* link);
        void endAcquire(WWLink* link);

    protected:
        void throwTaskStopIfNeed();

    protected:
        utils::IntrusiveDlist<WWLink> _links;
        std::size_t _linksAmount = 0;

    private:
        bool (* _canAcquire)(const Waitable* self) = nullptr;
        bool (* _tryAcquire)(Waitable* self) = nullptr;

    };
}
