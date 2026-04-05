/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include <dci/cmt/task/owner.hpp>
#include <dci/sbs/wire.hpp>
#include <dci/cmt/raisable.hpp>
#include <dci/utils/intrusiveDlist.hpp>
#include <memory>
#include <atomic>

namespace dci::poll::impl
{
    class TagForAll;
    class TagForReady;

    class Awaker final
        : public utils::IntrusiveDlistElement<Awaker, TagForAll>
        , public utils::IntrusiveDlistElement<Awaker, TagForReady>
    {
        Awaker(const Awaker&) = delete;
        void operator=(const Awaker&) = delete;

    public:
        Awaker(cmt::task::Owner* wokenOwner, cmt::Raisable* raisable, bool keepLoop);
        ~Awaker();

        bool keepLoop() const;

        sbs::Signal<> woken();
        bool ready() const;
        void emitWokenIfNeed();

        void setWokenOwner(cmt::task::Owner* wokenOwner);
        void resetWokenOwner();

        void setRaisable(cmt::Raisable* raisable);
        void resetRaisable();

        void wakeup();

    private:

        struct Woken
        {
            Awaker*     _owner;
            bool        _inProgress{};
            sbs::Wire<> _wire;

            Woken(Awaker* owner) : _owner{owner} {}
        };
        using WokenPtr = std::shared_ptr<Woken>;
        WokenPtr            _woken{std::make_shared<Woken>(this)};

        cmt::task::Owner*   _wokenOwner{};
        cmt::task::Owner    _localWokenOwner{};
        cmt::Raisable*      _raisable{};

        bool                _keepLoop{true};

        std::atomic_bool    _ready{};
    };
}
