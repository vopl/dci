/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <cstdint>

namespace dci::sbs
{
    class Subscription;
}

namespace dci::sbs::impl
{
    class Box;
    class Owner;

    class Subscription final
    {
    public:
        using Activator = void (*)(sbs::Subscription*, void*, std::uint_fast8_t);

        Subscription(Activator activator, Owner* owner);
        ~Subscription();

        void removeSelf();
        void activate(void* context, std::uint_fast8_t flags, bool deletionRequested);

    private:
        friend class Box;
        friend class Owner;

        Activator _activator = nullptr;

        uint32_t        _activationDepth = 0;
        bool            _deletionRequested = false;
        bool            _deletionActivated = false;

        Owner*          _owner      = nullptr;
        Subscription*   _prevInOwner= nullptr;
        Subscription*   _nextInOwner= nullptr;

        Box*            _box        = nullptr;
        Subscription*   _prevInBox  = nullptr;
        Subscription*   _nextInBox  = nullptr;
    };
}
